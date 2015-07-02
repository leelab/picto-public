#ifndef _TASK_CONFIG_H_
#define _TASK_CONFIG_H_

#include <QObject>
#include <QMap>
#include <QList>
#include <QMutex>
#include <QHash>

#include "../operator/OperatorPlotHandler.h"

#include "../common.h"

namespace Picto {
class DataViewElement;
class OperatorPlotHandler;
class OperatorPlot;
class NeuralDataListener;
class NeuralDataUnit;
/*! \brief There are many operations and data elements that need to be associated with a task as a whole, accessible by
 *	all elements of the task and with access to all elements of the task.  Objects of the TaskConfig class handle
 *	these types of operations and store this type of data.  This is a lower-scoped analog to the DesignConfig class, and
 *	much lighter-weight.
 *	
 *	\details Some of the types of operations that require access to/from all task Assets are: 
 *	- Configuring and accessing Task plot objects
 *	- (FUTURE) Uniquely gating Input vs Output on the NIDaq Digital lines
 *
 *	These and other general task configuration issues are handled by this class.  Every Picto Asset holds a pointer to
 *	the TaskConfig of the task to which it is attached and elements needing to be accessed through the TaskConfig add
 *	themselves to the TaskConfig object (which holds a weak pointer to them to avoid circular shared pointer issues).
 *	The TaskConfig of a particular Task can be accessed using its Asset::getTaskConfig() function.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API TaskConfig : public QObject
#else
class TaskConfig : public QObject
#endif
{
	Q_OBJECT
public:
	TaskConfig();
	virtual ~TaskConfig(){};

	void addObserverWidget(DataViewElement *owningAsset, QWidget *widget);

	void removeWidget(QWidget *widget);

	void reset();

	void requestUpdate();

	void setUpdateSignalEnabled(bool bEnabled = true);

	const QList<QWidget *> getWidgets() const;
	const QList<QString> getNames() const;

	const QString getName(QWidget *pWidget) const;

	void addObserver(DataViewElement *newAsset);

	void requestPlotHandler(QWeakPointer<OperatorPlot> plot, const QString plotPath);
	void cachePlotHandler(QSharedPointer<OperatorPlotHandler> handler, const QString plotPath );
	void clearPlotHandlers();

	void notifyAnalysisSelection(const QString &name, bool selected);

public:
	const ViewProperties getDisplayWidgetProperties(QWidget *widget) const;

	//! Used to set the size of the TaskDisplay for the ViewSelectionFrame
	void setTaskViewProperties(const ViewProperties props) { taskViewProperties_ = props; };
	//! Used to get the size of the TaskDisplay for the ViewSelectionFrame
	const ViewProperties getTaskViewProperties() const { return taskViewProperties_; };

	void addNeuralDataListener(QWeakPointer<NeuralDataListener> plot);
	void notifyNeuralDataListeners(const NeuralDataUnit &neuralData);
	void notifyBehavioralTime(double time);

signals:
	//! A signal sent whenever a viewer widget is added to the Task.
	void widgetAddedToMap(QWidget *pWidget);
	//! A signal sent whenever a viewer widget is removed from the Task.
	void widgetRemovedFromMap(QWidget *pWidget);

	//! A signal requesting a cahced plotHandler from the manager, if they exist.
	void plotHandlerRequested(const QString plotPath);
	//! A signal to the manager of the plotHandlers to cache a new plotHandler
	void requestCachePlotHandler(QSharedPointer<OperatorPlotHandler> handler, const QString plotPath);
	//! A signal to the manager of the plotHandlers to clear cached plotHandlers
	void requestClearPlotHandlers();

public slots:
	void receivePlotHandler(QSharedPointer<OperatorPlotHandler> handler, const QString plotPath);
	void managerConnectionEstablished(bool connected);

protected:
	//! A Map of widget pointers vs their names.  DANGEROUS?!?
	QMap<QWidget*,QString> widgetNameMap;
	//! A Multimap of analysis names vs its associated widgets.
	QMultiMap<QString, QWidget*> analysisWidgetMap;
	//! Current Analysis Dispaly states
	QMap<QString, bool> analysisDisplayState;
	//! Holds onto the view properties of passed in DataViewAssets.
	QMap<QWidget*, ViewProperties> displayProperties;

	//! Holds onto the Task's default view size.
	ViewProperties taskViewProperties_;

	//! Flag to enable/disable updates
	bool updateSignalEnabled_;
	//! A List of Assets waiting to be pinged for an update;
	QList<DataViewElement*> waitingAssets;
	//! Mutex for protecting the List of Assets waiting to be pinged for an update
	QMutex mtxWaitingAssetProtector;

	//! A map of operatorPlots that have requested PlotHandlers but haven't received a reply
	QMap<QString, QWeakPointer<OperatorPlot>> waitingOperatorPlots_;

	//! A list of registered NeuralDataListeners
	QList<QWeakPointer<NeuralDataListener>> neuralDataListeners_;

	void addWaitingAssets();

	enum ConnectionStatus {
		Unconnected = 0,
		Connected,
		ConnectionEnded,
		CONNECTION_STATUS_MAX
	} managerConnectionStatus_;
};

}; //namespace Picto

#endif
