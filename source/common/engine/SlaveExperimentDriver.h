#ifndef _SLAVEEXPERIMENTDRIVER_H_
#define _SLAVEEXPERIMENTDRIVER_H_
#include <QSharedPointer>
#include <QTime>
#include "../experiment/experiment.h"
#include "StateUpdater.h"
#include "propertytable.h"
#include "SlaveEventQueue.h"

#include "../common.h"

namespace Picto {

/*! \brief Connects a StateUpdater to an Experiment to drive an Experiment in slave mode.
 *	\details When a Picto Experiment runs in slave mode, the slave gets data from the master and 
 *	that data is delivered from a StateUpdater object.  The delivered data needs to be correctly transferred
 *	into the local Experiment with the correct initializations and the correct handling of the local Experiment's
 *	control flow.  That is the job of the SlaveExperimentDriver.  The SlaveExperimentDriver doesn't care if data 
 *	is coming from a running remote experiment or an old session file that is being played back locally.  It simply
 *	takes the data coming in from the StateUpdater and uses it to control the active Experiment.
 *	\note The Experiment's engine should be set to the Experiment used by this SlaveExperimentDriver in outside code.
 *	This is necessary since the Engine needs to be properly configured for the current render/run environment, and the
 *	SlaveExperimentDriver is reponsible only for sending data into the Experiment.  It knows nothing about the 
 *	current run environment.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SlaveExperimentDriver : public QObject
#else
class SlaveExperimentDriver : public QObject
#endif
{
	Q_OBJECT
public:
	SlaveExperimentDriver(QSharedPointer<Experiment> exp,QSharedPointer<StateUpdater> updater);
	virtual ~SlaveExperimentDriver(){};
signals:
	/*! \brief Emitted when the currently running task changes.  currTask is the name of the new Task.
	*	\details This can happen suddently sometimes even when it doesn't make sense in the StateMachine control flow.  For example, A RemoteViewer might lose its
	*	network connection briefly and then get it back after a different RemoteViewer changed the running task.  Since the RemoteViewer only ever polls the 
	*	current experimental state and not the entire history, it might find that a new task was started even though it never heard that the previous Tasks run
	*	ended.
	*/
	void taskChanged(QString currTask);
private:
	void renderFrame();
	void handleEvent(SlaveEvent& event);
	QSharedPointer<Experiment> experiment_;
	QSharedPointer<StateUpdater> updater_;
	QSharedPointer<DesignConfig> designConfig_;
	QSharedPointer<StateMachineElement> currElement_;
	SlaveEventQueue eventQueue_;
	QTime frameTimer_;
	bool renderingEnabled_;
	QString currTask_;
private slots:
	void masterRunStarting(QString taskName,QString runName);
	void masterRunEnding();

	void masterPropertyValueChanged(int propId, QString value);
	void masterPropertyInitValueChanged(int propId, QString value);
	void masterTransitionActivated(int transId);
	void masterFramePresented(double time);
	void masterRewardSupplied(double time,int duration,int channel);
	void masterSignalChanged(QString name,QStringList subChanNames,QVector<float> vals);
	void disableRendering(bool disable);
};


}; //namespace Picto

#endif
