#include <QtDebug>

#include "TaskConfig.h"
#include "../operator/DataViewElement.h"

#include "../memleakdetect.h"

namespace Picto
{

TaskConfig::TaskConfig()
	:updateSignalEnabled_(true)
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

	waitingAssets.push_back(newAsset);
}

/*! \brief Used to hook up a ObserverPlot with its plothandler.
 */
QSharedPointer<OperatorPlotHandler> TaskConfig::getPlotHandler(QString plotPath)
{
	if (cachedHandlers_.contains(plotPath))
	{
		return cachedHandlers_[plotPath];
	}

	return QSharedPointer<OperatorPlotHandler>();
}

/*! \brief Used to store an ObserverPlot's plothandler for later recovery.
 */
void TaskConfig::setPlotHandler(QString plotPath, QSharedPointer<OperatorPlotHandler> handler)
{
	cachedHandlers_[plotPath] = handler;
}

/*! \brief Clears all cached PlotHandlers.  This can potentially trigger their deletion.
 */
void TaskConfig::clearPlotHandlers()
{
	cachedHandlers_.clear();
}

/*! \brief Adds the passed in ObserverWidget to the TaskConfig.
 */
void TaskConfig::addObserverWidget(DataViewElement *owningAsset, QWidget *widget)
{
	Q_ASSERT(widget != nullptr);

	widgetNameMap[widget] = owningAsset->getTitle();
	QString analysisName = owningAsset->getParentAsset()->getName();
	analysisWidgetMap.insert(analysisName, widget);
	qDebug() << "Added widget for Analysis:" << analysisName;
	displayProperties.insert(widget, std::move(DisplayWidgetProperties(owningAsset->getDefaultViewSize())));

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
	qDebug() << "Notified of Analysis:" << name;

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

/*! \brief Returns a QList of the names of the current View Widgets.
*/
const QList<QString> TaskConfig::getNames() const
{
	return widgetNameMap.values();
}

const QString TaskConfig::getName(QWidget *pWidget) const
{
	if (widgetNameMap.contains(pWidget))
	{
		return widgetNameMap[pWidget];
	}

	return QString("");
}

const TaskConfig::DisplayWidgetProperties &TaskConfig::getDisplayWidgetProperties(QWidget *widget) const
{
	if (displayProperties.contains(widget))
	{
		return displayProperties[widget];
	}

	return DisplayWidgetProperties(DataViewSize::VIEW_SIZE_1x1);
}

}
