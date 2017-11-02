#include <QtDebug>

#include "TaskConfig.h"
#include "../operator/NeuralDataListener.h"
#include "../operator/DataViewElement.h"
#include "../operator/OperatorPlot.h"

#include "../memleakdetect.h"

namespace Picto
{

TaskConfig::TaskConfig()
	: updateSignalEnabled_(true), managerConnectionStatus_(Unconnected), NeuralTick_("audio/tick.wav"), selectedChannel_(0), selectedUnit_(0), listenerAdded_(false)
{
	reset();
}

//! Clears out all of this object's saved data.  Returning it to its initial state.
void TaskConfig::reset()
{
	QMutexLocker locker(&mtxWaitingAssetProtector);

	widgetNameMap.clear();
	waitingAssets.clear();

	updateSignalEnabled_ = true;
}

//! An exposed public function that triggers the waiting assets.
void TaskConfig::requestUpdate()
{
	addWaitingAssets();
}

//! Iterates over waiting assets and pings them for an update
void TaskConfig::addWaitingAssets()
{
	QMutexLocker locker(&mtxWaitingAssetProtector);

	foreach(DataViewElement *asset, waitingAssets)
	{
		asset->sendWidgetToTaskConfig();
	}

	waitingAssets.clear();
}

//! An addition of an observer widget that is deferred until addWaitingAssets() is called.
void TaskConfig::addObserver(DataViewElement *newAsset)
{
	QMutexLocker locker(&mtxWaitingAssetProtector);

	switch (managerConnectionStatus_)
	{
	case ConnectionStatus::Unconnected:
		waitingAssets.push_back(newAsset);
		break;
	case ConnectionStatus::Connected:
		newAsset->sendWidgetToTaskConfig();
		break;
	case ConnectionStatus::ConnectionEnded:
		qDebug() << "Tried to add DataViewElement to a dead TaskConfig (addObserver)";
		break;
	default:
		qDebug() << "Unrecognized ConnectionStatus (addObserver)";
	}
}

//! Send a message to store an ObserverPlot's plothandler for later recovery.
void TaskConfig::cachePlotHandler(QSharedPointer<OperatorPlotHandler> handler, const QString plotPath)
{
	switch (managerConnectionStatus_)
	{
	case ConnectionStatus::Unconnected:
		qDebug() << "Tried to cache a PlotHandler when not connected.";
		break;
	case ConnectionStatus::Connected:
		emit requestCachePlotHandler(handler, plotPath);
		break;
	case ConnectionStatus::ConnectionEnded:
		qDebug() << "Tried to cache a PlotHandler with a dead TaskConfig (cachePlotHandler)";
		break;
	default:
		qDebug() << "Unrecognized ConnectionStatus (cachePlotHandler)";
	}
}

/*! \brief Clears all cached PlotHandlers.  This can potentially trigger their deletion.
 */
void TaskConfig::clearPlotHandlers()
{
	if (managerConnectionStatus_ == ConnectionStatus::Connected)
		emit requestClearPlotHandlers();
}

//! OperatorPlots call this function to get their cached PlotHandler, if there is one.
void TaskConfig::requestPlotHandler(QWeakPointer<OperatorPlot> plot, const QString plotPath)
{
	waitingOperatorPlots_[plotPath] = plot;

	switch (managerConnectionStatus_)
	{
	case ConnectionStatus::Unconnected:
		break;
	case ConnectionStatus::Connected:
		emit plotHandlerRequested(plotPath);
		break;
	case ConnectionStatus::ConnectionEnded:
		qDebug() << "Tried to request from a dead TaskConfig (requestPlotHandler)";
		break;
	default:
		qDebug() << "Unrecognized ConnectionStatus (requestPlotHandler)";
	}

}

//! Response from manager of PlotHandlers bearing either the handler, or a null pointer
void TaskConfig::receivePlotHandler(QSharedPointer<OperatorPlotHandler> handler, const QString plotPath)
{
	if (waitingOperatorPlots_.contains(plotPath) && !waitingOperatorPlots_[plotPath].isNull())
	{
		waitingOperatorPlots_[plotPath].toStrongRef()->receivePlotHandler(handler);
		waitingOperatorPlots_.remove(plotPath);
	}
}

//! Signal sent when a PlotHandler manager connects and disconnects
void TaskConfig::managerConnectionEstablished(bool connected)
{
	bool justConnected = false;
	switch (managerConnectionStatus_)
	{
	case ConnectionStatus::Unconnected:
		if (connected)
		{
			managerConnectionStatus_ = ConnectionStatus::Connected;
			justConnected = true;
		}
		else
		{
			qDebug() << "Tried to break connection when No Connection enabled.";
		}
		break;
	case ConnectionStatus::Connected:
		if (connected)
		{
			qDebug() << "Tried to open connection when already enabled.";
		}
		else
		{
			managerConnectionStatus_ = ConnectionStatus::ConnectionEnded;
		}
		break;
	case ConnectionStatus::ConnectionEnded:
		if (connected)
		{
			//Reconnected
			managerConnectionStatus_ = ConnectionStatus::Connected;
		}
		else
		{
			qDebug() << "Tried to modify ended connection with connected =" << connected;
		}
		break;
	default:
		qDebug() << "Unrecognized ConnectionStatus (managerConnectionEstablished)";
	}

	if (justConnected)
	{
		QStringList plotPaths = waitingOperatorPlots_.keys();
		foreach(QString plotPath, plotPaths)
		{
			if (!waitingOperatorPlots_[plotPath].isNull())
			{
				emit plotHandlerRequested(plotPath);
			}
			else
			{
				waitingOperatorPlots_.remove(plotPath);
			}
		}
	}

}

/*! \brief Adds the passed in ObserverWidget to the TaskConfig.
 */
void TaskConfig::addObserverWidget(DataViewElement *owningAsset, QWidget *widget)
{
	Q_ASSERT(widget != nullptr);

	widgetNameMap[widget] = owningAsset->getTitle();
	QString analysisName = owningAsset->getParentAsset()->getName();
	analysisWidgetMap.insert(analysisName, widget);
	displayProperties.insert(widget, std::move(ViewProperties(owningAsset->getDefaultViewProperties())));

	if (!analysisDisplayState.contains(analysisName))
	{
		analysisDisplayState[analysisName] = false;
	}
	
	if (updateSignalEnabled_ && analysisDisplayState[analysisName])
	{
		emit widgetAddedToMap(widget);
	}
	
}

//! Receives information about which analyses are currently active.
void TaskConfig::notifyAnalysisSelection(const QString &name, bool selected)
{
	if (analysisDisplayState.contains(name) && analysisDisplayState[name] == selected)
		return;

	analysisDisplayState[name] = selected;

	if (analysisWidgetMap.contains(name))
	{
		QList<QWidget*> widgets = analysisWidgetMap.values(name);
		if (selected)
		{
			foreach(QWidget *widget, widgets)
			{
				emit widgetAddedToMap(widget);
			}
		}
		else
		{
			foreach(QWidget *widget, widgets)
			{
				emit widgetRemovedFromMap(widget);
			}
		}
	}
}

/*! \brief Removes the passed widget from the widgetNameMap, as well all assets pointing to it from the widgetMap.
 */
void TaskConfig::removeWidget(QWidget *widget)
{
	if (widgetNameMap.contains(widget))
	{
		if (updateSignalEnabled_)
		{
			emit widgetRemovedFromMap(widget);
		}

		//Remove from analysisWidgetMap, everywhere it appears
		QList<QString> keys = analysisWidgetMap.keys(widget);
		foreach(QString key, keys)
		{
			analysisWidgetMap.remove(key, widget);
		}

		//Remove the name from the name map		
		widgetNameMap.remove(widget);

		//for Plots
		if (displayProperties.contains(widget))		
			displayProperties.remove(widget);

		//removed next time alignEvent() is called
		//QList<QWeakPointer<NeuralDataListener>> neuralDataListeners_;
	}
}

/*! \brief Toggles whether widgetMapUpdated() is emitted every time a change occurs.
 */
void TaskConfig::setUpdateSignalEnabled(bool bEnabled)
{
	updateSignalEnabled_ = bEnabled;
}


/*! \brief Returns a QList of the current View Widgets.
 */
const QList<QWidget *> TaskConfig::getWidgets() const
{
	QList<QWidget*> activeWidgets;

	foreach(QString name, analysisDisplayState.keys())
	{
		if (analysisDisplayState[name])
		{
			activeWidgets.append(analysisWidgetMap.values(name));
		}
	}

	return activeWidgets;
}

/*!	Returns a QList of the names of the current View Widgets.
 */
const QList<QString> TaskConfig::getNames() const
{
	return widgetNameMap.values();
}

/*!	Returns the stored name for the passsed-in widget pointer.
 */
const QString TaskConfig::getName(QWidget *pWidget) const
{
	if (widgetNameMap.contains(pWidget))
	{
		return widgetNameMap[pWidget];
	}

	return QString("");
}


/*!	Returns the cached ViewProperties associated with the passed-in displayWidget
 */
const ViewProperties TaskConfig::getDisplayWidgetProperties(QWidget *widget) const
{
	if (displayProperties.contains(widget))
	{
		return displayProperties[widget];
	}

	return ViewProperties();
}

/*!	Registers a NeuralDataListener to receive live NeuralData updates.
 */
void TaskConfig::addNeuralDataListener(QWeakPointer<NeuralDataListener> listener)
{
	listenerAdded_ = true;
	if (!neuralDataListeners_.contains(listener) && !listener.isNull())
	{
		neuralDataListeners_.push_back(listener);
	}
}
void TaskConfig::removeNeuralDataListeners()
{
	if (listenerAdded_)
	{
		QList<QWeakPointer<NeuralDataListener>>::iterator it = neuralDataListeners_.begin();
		while (it != neuralDataListeners_.end())
		{
				it = neuralDataListeners_.erase(it);
		}
	}
}
/*!	Distributes neuralData to registered listeners
 */
void TaskConfig::notifyNeuralDataListeners(const NeuralDataUnit &neuralData)
{
		//Emit a sound for each spike detected
		if ((selectedChannel_ == 0) || ((neuralData.getChannel() == selectedChannel_) && ((neuralData.getUnit() == selectedUnit_) || (selectedUnit_ == 0))))
		{
			//play sound
			NeuralTick_.play();

			//record
			emit spikeAdded(neuralData.getChannel(), neuralData.getUnit(), neuralData.getFittedtime());
		}

	bool bCleanupRequired = false;
	foreach(QWeakPointer<NeuralDataListener> listener, neuralDataListeners_)
	{
		if (listener.isNull())
		{
			bCleanupRequired = true;
		}
		else
		{
			listener.data()->addSpikeData(neuralData);
		}
	}

	if (bCleanupRequired)
	{
		QList<QWeakPointer<NeuralDataListener>>::iterator it = neuralDataListeners_.begin();
		while (it != neuralDataListeners_.end())
		{
			if (it->isNull())
			{
				it = neuralDataListeners_.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}
void TaskConfig::setSelectedNeural(int channel, int unit)
	{
		selectedChannel_ = channel;
		selectedUnit_ = unit;
	}

void TaskConfig::alignPlot(int alignID)
{
	bool bCleanupRequired = false;
	foreach(QWeakPointer<NeuralDataListener> listener, neuralDataListeners_)
	{
		if (listener.isNull())
		{
			bCleanupRequired = true;
		}
		else
		{
			listener.data()->alignPlot(alignID);
		}
	}

	if (bCleanupRequired)
	{
		QList<QWeakPointer<NeuralDataListener>>::iterator it = neuralDataListeners_.begin();
		while (it != neuralDataListeners_.end())
		{
			if (it->isNull())
			{
				it = neuralDataListeners_.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}
void TaskConfig::eventAdded(int eventID)
{
	bool bCleanupRequired = false;
	foreach(QWeakPointer<NeuralDataListener> listener, neuralDataListeners_)
	{
		if (listener.isNull())
		{
			bCleanupRequired = true;
		}
		else
		{
			listener.data()->addEventData(eventID);
		}
	}

	if (bCleanupRequired)
	{
		QList<QWeakPointer<NeuralDataListener>>::iterator it = neuralDataListeners_.begin();
		while (it != neuralDataListeners_.end())
		{
			if (it->isNull())
			{
				it = neuralDataListeners_.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}
void TaskConfig::scriptContAdded(int scriptID)
{
	bool bCleanupRequired = false;
	foreach(QWeakPointer<NeuralDataListener> listener, neuralDataListeners_)
	{
		if (listener.isNull())
		{
			bCleanupRequired = true;
		}
		else
		{
			listener.data()->addScriptCont(scriptID);
		}
	}

	if (bCleanupRequired)
	{
		QList<QWeakPointer<NeuralDataListener>>::iterator it = neuralDataListeners_.begin();
		while (it != neuralDataListeners_.end())
		{
			if (it->isNull())
			{
				it = neuralDataListeners_.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}
}
