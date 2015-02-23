/*! \file PictoEngine.h
 * \brief
 */
#pragma once
#ifndef _PICTOENGINE_H_
#define _PICTOENGINE_H_

#include <QSharedPointer>
#include <QFuture>
#include <QUuid>
#include <QList>
#include <QMutex>
#include <QNetworkInterface>
#include <QMap>

#include "../common.h"
#include "../compositor/RenderingTarget.h"
//#include "../task/Task.h"
//#include "../experiment/Experiment.h"
#include "../network/CommandChannel.h"
#include "../protocol/ProtocolCommand.h"
#include "GenericInput.h"
#include "SignalChannel.h"
#include "DoubletSignalChannel.h"
#include "../iodevices/RewardController.h"
#include "../iodevices/OutputSignalController.h"
#include "../iodevices/EventCodeGenerator.h"
#include "../storage/PropertyDataUnitPackage.h"
#include "../storage/StateDataUnitPackage.h"
#include "../storage/BehavioralDataUnitPackage.h"
#include "../storage/InputDataUnitPackage.h"
#include "../storage/StateDataUnit.h"
#include "../storage/RewardDataUnit.h"
#include "../storage/TaskRunDataUnit.h"
#include "../storage/DesignConfig.h"
#include "StateUpdater.h"
#include "propertytable.h"
#include "ControlPanelInterface.h"
#include "../playback/LiveFrameReader.h"
#include "../playback/LiveLfpReader.h"
#include "../playback/LiveRewardReader.h"
#include "../playback/LiveSpikeReader.h"
#include "../playback/LiveSignalReader.h"
#include "../controlelements/FrameTimerFactory.h"

namespace Picto {
	namespace Engine {

/*! \brief The namespace used for the PictoEngineTimingType enumeration
 *
 *	\note This namespace does not actually appear to be used.  It should probably be deleted.
 *	
 * \details This namespace is used to improve code readability; it simply contains the PictoEngineTimingType
 * enum.  This necessitates qualifying an enumerated type in context.  For example, instead of
 * referring to an enum as "brief" in your code, you would have to refer to it as ExampleType::brief.
 * In so doing, it is unambiguous what you are referring to.  To refer to the enum as a type itself 
 * you would simply repeat the namespace (in the example above it would be ExampleType::ExampleType).
 *
 */
namespace PictoEngineTimingType
{
	/*! \brief An enumerated type containing timing control options for PictoEngine objects
	 *
	 *	\note This enum does not actually appear to be used.  It should probably be deleted.
	 * \details PictoEngine objects can be set to have precise timing control, or to ignore frame intervals
	 * (to catch up a playback, allow for fast forward and rewind behaviors, enable generation of movie
	 * files, allow analysis of replayed data, etcetera).  PictoEngineTimingType enumerates the
	 * valid timing options.
	 * 
	 */
	typedef enum
	{
		precise,				/*!< Utilize precise timing */
		ignoreFrameIntervals	/*!< Ignore frame intervals during processing */
	} PictoEngineTimingType;
}

/*! \brief Runs a Picto Experiment.
 *
 *	As implied by the name, the PictoEngine runs the Experiment.  It actually does far too much for one class and should probably be split into a number 
 *	of smaller classes with one high level class that manages interactions between the smaller classes.  In its current incarnation, however, the PictoEngine's most
 *	important function is taking an Experiment object and executing its contained StateMachines by rendering to one or more RenderingTarget derived 
 *	objects.  It also handles the other pieces of experiment execution, such as reward delivery, alignment event delivery, output voltage signal production, input
 *	signal handling, interfacing with control panel applications (ie. Picto front panel), etc.  The PictoEngine can be assigned a CommandChannel with which to 
 *	communicate all changes in experimental state to a PictoServer.  It can be run in master or slave mode, where the master engine drives the experiment according 
 *	to user input and the slave engine drives the experiment using data from an external master retrieved from the PictoServer.  The engine can also be run in 
 *	operator or subject mode, which defines visibility of experimental graphics (graphics can be set visible for the experiment operator (tester), subject (testee) 
 *	or both).
 *	
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PictoEngine : public QObject
#else
class PictoEngine : public QObject
#endif
{
		Q_OBJECT
public:

	PictoEngine();

	/*! Sets the timing control for the PictoEngine to \a _timingType.
		\note This function does not appear to be used.  It's probably worth getting rid of it.
	    \param _timingType one of the PictoEngineTimingType::PictoEngineTimingType
		       enum values specifying the desired timing
		\return none
	 */
	void setTimingControl(PictoEngineTimingType::PictoEngineTimingType _timingType);

	/*! \brief Sets this PictoEngine to run the Experiment in Exclusive mode.
	 *	\details Exclusive mode indicates that the running experiment wants as much timing control as possible, as such
	 *	control is never passed back to the Qt event loop.
	 *	\note Instead of using a separate exclusiveMode flag here, we should consider using !slaveMode() && !operatorIsUser().
	 *	Also, since computers have sped up significantly since this was designed, we should really explore rewriting the Engine and 
	 *	StateMachineElements such that a Picto Experiment can be run as part of an event loop.  Doing this would vastly decrease
	 *	a great deal of comlexity in Picto.
	 */
	void setExclusiveMode(bool mode) { bExclusiveMode_ = mode; };
	/*! \brief Returns true if this PictoEngine is running in Exclusive mode, false otherwise.
	 *	\details Exclusive mode indicates that the running experiment wants as much timing control as possible, as such
	 *	control is never passed back to the Qt event loop.
	 *	\note Instead of using a separate exclusiveMode flag here, we should consider using !slaveMode() && !operatorIsUser().
	 *	Also, since computers have sped up significantly since this was designed, we should really explore rewriting the Engine and 
	 *	StateMachineElements such that a Picto Experiment can be run as part of an event loop.  Doing this would vastly decrease
	 *	a great deal of comlexity in Picto.
	 */
	bool getExclusiveMode() { return bExclusiveMode_; };

	QList<QSharedPointer<RenderingTarget> > getRenderingTargets();
	void addRenderingTarget(QSharedPointer<RenderingTarget> target);

	QList<QSharedPointer<ControlPanelInterface>> getControlPanels();
	void addControlPanel(QSharedPointer<ControlPanelInterface> controlPanel);

	QSharedPointer<SignalChannel> getSignalChannel(QString name);
	void addSignalChannel(QSharedPointer<SignalChannel> channel);
	void startAllSignalChannels();
	void stopAllSignalChannels();
	void emptySignalChannels();
	void markTaskRunStart(QString name);
	void markTaskRunStop();

	/*! \brief Sets the event code generator that should be used to send alignment codes to a neural data acquisition system for timing alignment purposes.
	 *	\details Picto aligns timestamps from the behavioral and neural systems by sending numerical codes over a physical line from the behavioral system
	 *	to the neural system.  The behavioral system marks when the signal was sent, and the neural system marks when it was recieved.  Using this data, the
	 *	server can align the clocks of the two systems.  Depending on the underlying hardware of the computer running Picto, different alignment Code Generators
	 *	will need to be used.  This function tells the PictoEngine which one to use.
	 *	\note For historical reasons, what we now refer to as an Alignment code generator is still referred to as an event code generator throughout most of 
	 *	Picto.  This will likely lead to some confusion and should be fixed.
	 */
	void setEventCodeGenerator(QSharedPointer<EventCodeGenerator> eventCodeGenerator) { eventCodeGenerator_ = eventCodeGenerator; };
	double generateEvent(unsigned int eventCode);

	/*! \brief Sets the RewardConroller that will be used to supply rewards during this experiment
	 *	\details Depending on the hardware underlying the computer running a Picto experiment, or whether we are running
	 *	a real experiment or a test, different code will be needed to supply rewards.  The RewardController abstracts those
	 *	implementation details such that the Engine need only worry about interfacing with it.
	 */
	void setRewardController(QSharedPointer<RewardController> rewardController) { rewardController_ = rewardController; };
	void giveReward(int channel, int quantity, int minRewardPeriod);
	QList<QSharedPointer<RewardDataUnit>> getDeliveredRewards();

	/*! \brief Sets the OutputSignalController that will be used to change voltage signals output from the experiment.
	 *	\details Depending on the hardware underlying the computer running a Picto experiment, or whether we are running
	 *	a real experiment or a test, different code will be needed to change output voltages.  The OutputSignalController abstracts those
	 *	implementation details such that the Engine need only worry about interfacing with it.
	 */
	void setOutputSignalController(QString port, QSharedPointer<OutputSignalController> outSigController) { if(!outSigController)return; outSigControllers_[port] = outSigController; };
	void setOutputSignalValue(QString port, int pinId, QVariant value);
	void enableOutputSignal(QString port, int pinId, bool enable);

	QSharedPointer<PropertyDataUnitPackage> getChangedPropertyPackage();
	//QSharedPointer<PropertyDataUnitPackage> getChangedInitPropertyPackage();
	//void updatePropertiesFromServer();
	/*! \brief Clears all of the latest changes stored in this engine's PropertyDataUnitPackage.
	 *	\sa getChangedPropertyPackage()
	 */
	void clearChangedPropertyPackages(){propPackage_.clear();};

	void addStateTransitionForServer(QSharedPointer<Transition> stateTrans);
	QSharedPointer<StateDataUnitPackage> getStateDataPackage();

	QList<QSharedPointer<BehavioralDataUnitPackage>> getBehavioralDataPackages();
	QList<QSharedPointer<InputDataUnitPackage>> getInputDataPackages();

	void reportNewFrame(double frameTime,int runningStateId);
	void setLastFrame(qulonglong frameId);
	/*! \brief Returns the frame ID of the latest frame presented.*/
	qulonglong getLastFrameId(){return lastFrameId_;};

	bool updateCurrentStateFromServer();

	bool setDataCommandChannel(QSharedPointer<CommandChannel> commandChannel);
	QSharedPointer<CommandChannel> getDataCommandChannel();

	bool setUpdateCommandChannel(QSharedPointer<CommandChannel> commandChannel);
	QSharedPointer<CommandChannel> getUpdateCommandChannel();

	bool setFrontPanelCommandChannel(QSharedPointer<CommandChannel> commandChannel);
	QSharedPointer<CommandChannel> getFrontPanelCommandChannel();

	bool setFrontPanelEventChannel(QSharedPointer<CommandChannel> commandChannel);
	QSharedPointer<CommandChannel> getFrontPanelEventChannel();

	void setPropertyTable(QSharedPointer<PropertyTable> propTable);
	/*! \brief Returns this engine's PropertyTable
	 *	\sa setPropertyTable()
	 */
	QSharedPointer<PropertyTable> getPropertyTable(){return propTable_;};
	void sendAllPropertyValuesToServer();

	void setSessionId(QUuid sessionId);
	/*! \brief Returns the Session ID of the currently loaded session.*/
	QUuid getSessionId() { return sessionId_; };

	/*! \brief Returns the name of the system on which this PictoEngine's experiment is running.
	 *	\sa setName()
	 */
	QString getName() { return name_; };

	/*! \brief Tells the PictoEngine if the current experiment user is the operator (ie. Tester).
	 *	\details If false is entered, the current experiment user is the subject (ie. Testee).
	 *	The experiment user is set as the Operator in the RemoteViewer or if desired in the TestViewer.
	 *	Typically, the operator sees more detail about an experiment that is not shown to the test subject.
	 */
	void setOperatorAsUser(bool operatorMode = true){userIsOperator_ = operatorMode;};
	/*! \brief Returns a boolean indicating if the operator is the experiment user or not.
	 *	\sa setOperatorAsUser()
	 */
	bool operatorIsUser(){return userIsOperator_;};
	/*! \brief Tells the PictoEngine if it should be running in slave mode.
	 *	\details In slave mode, experimental scripts don't run, all property updates and control flow details are handled by 
	 *	reading in the latest state from a master.  In a live experiment, the Pictobox is the master, it sends data to the Picto Server,
	 *	the Remote Viewer on the workstation is the slave and it gathers the latest State data by polling the Picto Server for the 
	 *	data sent in by the Pictobox.
	 */
	void setSlaveMode(bool mode) { slave_ = mode; };
	/*! \brief Returns true if this engine is running in slave mode, false if its running in master mode.*/
	bool slaveMode() { return slave_; }
	/*! \brief DOES NOT APPEAR TO BE IMPLEMENTED.  SHOULD BE REMOVED.
	 *	\details If disabled, Init properties of the slave experiment will not be synchronized with those of the master.
	 *	By default, they are synchronized
	 */
	void syncInitPropertiesForSlave(bool enable){syncInitProperties_ = enable;};
	/*! \brief Sets the Controller::FrameTimerFactory for this experiment.
	 *	\details Since frame timers in the same experiment all need to be updated each time a new frame comes in, we use a 
	 *	FrameTimerFactory foreach experiment that is responsible for creating the FrameTimers.  This allows us to set the 
	 *	FrameTImers up such that they get their frame timing data from a single place, and we only need to update one object
	 *	when a new frame time comes in.
	 */
	void setFrameTimerFactory(QSharedPointer<Controller::FrameTimerFactory> frameTimerFactory){frameTimerFactory_ = frameTimerFactory;};

	//Setup Data Readers
	//When running in test mode, data has to be added to data readers as it comes in for the purposes of Analysis
	/*! \brief Sets a LiveFrameReader to this PictoEngine so that it can be updated during the course of the experiment and used
	 *	for live analysis.
	 *	\details This is useful for testing experimental/analysis designs.
	 */
	void setFrameReader(QSharedPointer<LiveFrameReader> frameReader){frameReader_ = frameReader;};
	/*! \brief Sets a LiveLfpReader to this PictoEngine so that it can be updated during the course of the experiment and used
	 *	for live analysis.
	 *	\details This is useful for testing experimental/analysis designs.
	 */
	void setLfpReader(QSharedPointer<LiveLfpReader> lfpReader){lfpReader_ = lfpReader;};
	/*! \brief Sets a LiveRewardReader to this PictoEngine so that it can be updated during the course of the experiment and used
	 *	for live analysis.
	 *	\details This is useful for testing experimental/analysis designs.
	 */
	void setRewardReader(QSharedPointer<LiveRewardReader> rewardReader){rewardReader_ = rewardReader;};
	/*! \brief Sets a LiveSpikeReader to this PictoEngine so that it can be updated during the course of the experiment and used
	 *	for live analysis.
	 *	\details This is useful for testing experimental/analysis designs.
	 */
	void setSpikeReader(QSharedPointer<LiveSpikeReader> spikeReader){spikeReader_ = spikeReader;};
	/*! \brief Sets a LiveSignalReader to this PictoEngine with the input name so that it can be updated during the course of the experiment and used
	 *	for live analysis.
	 *	\details This is useful for testing experimental/analysis designs.
	 */
	void setSignalReader(QString name, QSharedPointer<LiveSignalReader> signalReader){signalReaders_[name.toLower()] = signalReader;};
	/*! \brief Removes all LiveSignalReader readers that were previously added to this Engine.
	 *	\sa setSignalReader()
	 */
	void clearSignalReaders(){signalReaders_.clear();};
	
	/*! \brief An enum used track requested changes in the PictoEngine run state.
	 */
	enum {
		NoCommand,	//!< No active command 
		PlayEngine, //!< Start running an Experimental task
		StopEngine, //!< Stop the currently running task
		PauseEngine	//!< Pause the currently running task the next time a PausePoint is reached in the control flow.
	};
	/*! \brief Used by experimental elements to programatically request that a pause be triggered.
	 *	\note If experimental elements simply called pause() the experiment management classes (ie. ComponentStatusManager)
	 *	wouldn't have any way to know that the experiment is paused and the different elements of the system would not be
	 *	properly synchronized.  For this reason, experimental elements simply call requestPause() and outside elements
	 *	can then connect to the pauseRequested() signal so that they can call pause() themselves.
	 */
	void requestPause(){emit pauseRequested();};
	int getEngineCommand();
public slots:
	void stop();
	/*! \brief Changes the engine command to PlayEngine so that a execution will begin/resume.*/
	void play() { engineCommand_ = PlayEngine;};
	/*! \brief Changes the engine command to PauseEngine so that execution will pause when it next reaches a pause point.*/
	void pause(){ engineCommand_ = PauseEngine;};
	void setName(QString name);
	void setRewardDuration(int controller, int duration);
	void setFlushDuration(int controller, int duration);
	void giveReward(int channel);
	void flushRequest(int channel);

signals:
	/*! \brief Emitted when an experimental element wants the task to pause.
	 *	\details Whoever is usually responsible for pausing should connect to this and call pause() when this is triggered.
	 *	We probably should have done this in a cleaner way... so you can think about that when you have time.
	 */
	void pauseRequested();
	/*! \brief Presumably this was going to be emitted when the first phosphor appeared on the experimental display.  In practice, it is 
	 *	not being emitted and nothing seems to use it.
	 */
	void firstPhosphorOccured();
	/*! \brief Emitted when the reward duration changes on the input controller to the input duration in milliseconds.*/
	void rewardDurationChanged(int controller, int duration);
	/*! \brief Emitted when the flush duration changes on the input controller to the input duration in seconds.*/
	void flushDurationChanged(int controller, int duration);
	/*! \brief Emitted when setName() is called on the PictoEngine.*/
	void nameChanged(QString name);
	/*! \brief This does not appear to ever be emitted or used.  It should probably just be deleted.*/
	void slaveTimeChanged(double time);
	/*! \brief Emitted when the test subject presses the escape key.
	 *	\details This is useful for situations when we want a subject to be able to escape out of an experiment.  In particular, in the Directory
	 *	where the application runs in full screen mode and automatically grabs focus, we need an escape signal to be able to close the application
	 *	for maintenance purposes.
	 */
	void escapePressed();
private:
	QHostAddress getIpAddress();
	//QSharedPointer<Experiment> experiment_;
	PictoEngineTimingType::PictoEngineTimingType timingType_;
	bool bExclusiveMode_;
	
	QSharedPointer<EventCodeGenerator> eventCodeGenerator_;
	QMap<QString, QSharedPointer<SignalChannel> > signalChannels_;
	QList<QSharedPointer<RenderingTarget> > renderingTargets_;
	QList<QSharedPointer<ControlPanelInterface>> controlPanelIfs_;
	QSharedPointer<RewardController> rewardController_;
	QMap<QString,QSharedPointer<OutputSignalController>> outSigControllers_;
	QSharedPointer<PropertyDataUnitPackage> propPackage_;
	QSharedPointer<StateDataUnitPackage> stateDataPackage_;
	QSharedPointer<CommandChannel> dataCommandChannel_;		//Used for sending data to the server
	QSharedPointer<CommandChannel> updateCommandChannel_;	//Used for sending everything except data
	QSharedPointer<CommandChannel> fpCommandChannel_;		//Used for receiving commands from the front panel
	QSharedPointer<CommandChannel> fpEventChannel_;			//Used for sending event data to the front panel
	QSharedPointer<PropertyTable> propTable_;
	QList<QSharedPointer<RewardDataUnit>> deliveredRewards_;
	QVector<int> rewardDurations_;
	QVector<int> flushDurations_;
	bool taskRunStarting_;
	bool taskRunEnding_;
	QString taskRunName_;
	QSharedPointer<TaskRunDataUnit> taskRunUnit_;
	QSharedPointer<DesignConfig> designConfig_;
	QMutex rewardListMutex_;
	QMutex rewardMutex_;

	QUuid sessionId_;
	QString name_;

	QSharedPointer<Controller::FrameTimerFactory> frameTimerFactory_;

	//Data Readers
	//When running in test mode, data has to be added to data readers as it comes in for the purposes of Analysis
	QSharedPointer<LiveFrameReader> frameReader_;
	QSharedPointer<LiveLfpReader> lfpReader_;
	QSharedPointer<LiveRewardReader> rewardReader_;
	QSharedPointer<LiveSpikeReader> spikeReader_;
	QHash<QString,QSharedPointer<LiveSignalReader>> signalReaders_;

	int engineCommand_;

	QSharedPointer<StateUpdater> stateUpdater_;
	bool slave_;
	bool userIsOperator_;
	bool syncInitProperties_;
	//QSharedPointer<BehavioralDataUnitPackage> currBehavUnitPack_;
	//QSharedPointer<BehavioralDataUnit> currBehavUnit_;
	qulonglong lastFrameId_;
	QHostAddress ipAddress_;

private slots:
	void addChangedPropertyValue(Property* changedProp);
	void addChangedPropertyInitValue(Property* changedProp);
	void firstPhosphorOperations(double frameTime);
};

	}; //namespace Engine
}; //namespace Picto

#endif
