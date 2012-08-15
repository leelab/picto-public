/*! \file PictoEngine.h
 * \ingroup picto_engine
 * \brief
 */

#ifndef _PICTOENGINE_H_
#define _PICTOENGINE_H_

#include <QSharedPointer>
#include <QFuture>
#include <QUuid>
#include <QList>
#include <QMutex>
#include <QNetworkInterface>

#include "../common.h"
#include "../compositor/RenderingTarget.h"
//#include "../task/Task.h"
//#include "../experiment/Experiment.h"
#include "../network/CommandChannel.h"
#include "../protocol/ProtocolCommand.h"
#include "SignalChannel.h"
#include "../iodevices/RewardController.h"
#include "../iodevices/EventCodeGenerator.h"
#include "../storage/PropertyDataUnitPackage.h"
#include "../storage/StateDataUnitPackage.h"
#include "../storage/BehavioralDataUnitPackage.h"
#include "../storage/StateDataUnit.h"
#include "../storage/RewardDataUnit.h"
#include "../storage/TaskRunDataUnit.h"
#include "../storage/experimentconfig.h"
#include "propertytable.h"
#include "ControlPanelInterface.h"

namespace Picto {
	namespace Engine {

/*! \addtogroup picto_engine
 * @{
 */

/*! \brief The namespace used for the PictoEngineTimingType enumeration
 *
 * This namespace is used to improve code readability; it simply contains the PictoEngineTimingType
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
	 * PictoEngine objects can be set to have precise timing control, or to ignore frame intervals
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

/*! \brief Executes state machines contained in Experiment objects
 *
 * The PictoEngine object takes an Experiment object and executes its contained state machines by rendering to one or
 * more RenderingTarget derived objects.  The PictoEngine can be set to have precise timing control, or to ignore
 * frame intervals (to catch up a playback, allow for fast forward and rewind behaviors, enable generation of movie
 * files, allow analysis of replayed data, etcetera).  The storage for behavioral data accomodates out of order
 * receipt.  Additionally a PictoEngine can be assigned a CommandChannel with which to communicate with a
 * PictoServer instance.
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
	    \param _timingType one of the PictoEngineTimingType::PictoEngineTimingType
		       enum values specifying the desired timing
		\return none
	 */
	void setTimingControl(PictoEngineTimingType::PictoEngineTimingType _timingType);

	void setExclusiveMode(bool mode) { bExclusiveMode_ = mode; };
	bool getExclusiveMode() { return bExclusiveMode_; };

	//bool loadExperiment(QSharedPointer<Experiment> experiment);
	//void clearExperiment();

	//bool runTask(QString taskName);

	//The engine commands are only used when the engine is being run locally
	//They should also only be used by the master of the engine (ie. The ComponentStatusManager,
	//or the remoteviewer/testviewer).  If an experiment itself wants to pause, it should use
	//the requestPause() command, which will Signal to the engine master that it should make
	//a pause happen.  This way everything is in sync, and the engine never things one thing 
	//while the engine master thinks something else.
	enum {NoCommand, PlayEngine, StopEngine, PauseEngine};
	void play() { engineCommand_ = PlayEngine;};
	void pause(){ engineCommand_ = PauseEngine;};
	void requestPause(){emit pauseRequested();};
	void stop();
	int getEngineCommand();

	QList<QSharedPointer<RenderingTarget> > getRenderingTargets();
	void addRenderingTarget(QSharedPointer<RenderingTarget> target);
	bool hasVisibleRenderingTargets();

	QList<QSharedPointer<ControlPanelInterface>> getControlPanels();
	void addControlPanel(QSharedPointer<ControlPanelInterface> controlPanel);

	QSharedPointer<SignalChannel> getSignalChannel(QString name);
	void addSignalChannel(QSharedPointer<SignalChannel> channel);
	void startAllSignalChannels();
	void stopAllSignalChannels();
	void markTaskRunStart(QString name);
	void markTaskRunStop();

	void setEventCodeGenerator(QSharedPointer<EventCodeGenerator> eventCodeGenerator) { eventCodeGenerator_ = eventCodeGenerator; };
	//Sends the event code and returns the time in seconds
	//that the event code was triggered before the function returned.  The idea here
	//is that in general, most EventCodeGenerators are going to need to hold a signal
	//high for some number of microseconds before returning.  In order to get an
	//accurage timestamp, we will record the time when this function returns minus
	//the value that it returns.  We can't simply use the time at which the function
	//is called because setup times may vary and are typically going to be harder to
	//estimate.
	double generateEvent(unsigned int eventCode);

	void setRewardController(QSharedPointer<RewardController> rewardController) { rewardController_ = rewardController; };
	void giveReward(int channel, int quantity, int minRewardPeriod);
	QList<QSharedPointer<RewardDataUnit>> getDeliveredRewards();

	//! \brief Retrieves the latest package of changed properties.
	//! Note that a package can only be retrieved once after which a new package is created.
	QSharedPointer<PropertyDataUnitPackage> getChangedPropertyPackage();
	//void updatePropertiesFromServer();
	void clearChangedPropertyPackage(){propPackage_.clear();};

	void addStateTransitionForServer(QSharedPointer<Transition> stateTrans);
	QSharedPointer<StateDataUnitPackage> getStateDataPackage();
	void clearStateDataPackage(){propPackage_.clear();};

	QList<QSharedPointer<BehavioralDataUnitPackage>> getBehavioralDataPackages();

	void reportNewFrame(double frameTime,int runningStateId);
	void setLastFrame(qulonglong frameId);
	qulonglong getLastFrameId(){return lastFrameId_;};

	bool updateCurrentStateFromServer();
	void setRunningPath(QString path);
	QString getRunningPath(){return runningPath_;};
	QString getServerPathUpdate();

	bool setDataCommandChannel(QSharedPointer<CommandChannel> commandChannel);
	QSharedPointer<CommandChannel> getDataCommandChannel();

	bool setUpdateCommandChannel(QSharedPointer<CommandChannel> commandChannel);
	QSharedPointer<CommandChannel> getUpdateCommandChannel();

	bool setFrontPanelCommandChannel(QSharedPointer<CommandChannel> commandChannel);
	QSharedPointer<CommandChannel> getFrontPanelCommandChannel();

	bool setFrontPanelEventChannel(QSharedPointer<CommandChannel> commandChannel);
	QSharedPointer<CommandChannel> getFrontPanelEventChannel();

	void setPropertyTable(QSharedPointer<PropertyTable> propTable);
	QSharedPointer<PropertyTable> getPropertyTable(){return propTable_;};
	void sendAllPropertyValuesToServer();

	void setSessionId(QUuid sessionId);
	QUuid getSessionId() { return sessionId_; };


	QString getName() { return name_; };

	void setOperatorAsUser(bool operatorMode = true){userIsOperator_ = operatorMode;};
	bool operatorIsUser(){return userIsOperator_;};
	void setSlaveMode(bool mode, CommandChannel *serverChan) { slave_ = mode; slaveCommandChannel_ = serverChan; };
	bool slaveMode() { return slave_; }
	void setLastTimePropertiesRequested(QString time){lastTimePropChangesRequested_ = time;};
	void resetLastTimeStateDataRequested(){lastTimeStateDataRequested_ = "0.0";runningPath_="";currStateUnit_.clear();firstCurrStateUpdate_ = true;};
	double getLastTimeStateDataRequested(){return lastTimeStateDataRequested_.toDouble();};
	void setExperimentConfig(QSharedPointer<ExperimentConfig> expConfig){expConfig_ = expConfig;currStateUnit_.clear();};
	QSharedPointer<ExperimentConfig> getExperimentConfig(){return expConfig_;};

public slots:
	void setName(QString name);
	void setRewardDuration(int controller, int duration);
	void setFlushDuration(int controller, int duration);
	void giveReward(int channel);
	void flushRequest(int channel);

signals:
	void pauseRequested();
	void firstPhosphorOccured();
	void rewardDurationChanged(int controller, int duration);
	void flushDurationChanged(int controller, int duration);
	void nameChanged(QString name);
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
	QSharedPointer<PropertyDataUnitPackage> propPackage_;
	QSharedPointer<StateDataUnitPackage> stateDataPackage_;
	QSharedPointer<CommandChannel> dataCommandChannel_;		//Used for sending data to the server
	QSharedPointer<CommandChannel> updateCommandChannel_;	//Used for sending everything except data
	QSharedPointer<CommandChannel> fpCommandChannel_;		//Used for receiving commands from the front panel
	QSharedPointer<CommandChannel> fpEventChannel_;			//Used for sending event data to the front panel
	CommandChannel *slaveCommandChannel_;	//Used for communicating with the server in slave mode
	QSharedPointer<PropertyTable> propTable_;
	QList<QSharedPointer<RewardDataUnit>> deliveredRewards_;
	QVector<int> rewardDurations_;
	QVector<int> flushDurations_;
	bool taskRunStarting_;
	bool taskRunEnding_;
	QString taskRunName_;
	QSharedPointer<TaskRunDataUnit> taskRunUnit_;
	QSharedPointer<ExperimentConfig> expConfig_;
	bool firstCurrStateUpdate_;
	QMutex rewardListMutex_;
	QMutex rewardMutex_;

	QUuid sessionId_;
	QString name_;

	int engineCommand_;

	bool slave_;
	bool userIsOperator_;
	QString lastTimePropChangesRequested_;
	QString lastTimeStateDataRequested_;
	QSharedPointer<BehavioralDataUnitPackage> currBehavUnitPack_;
	QSharedPointer<BehavioralDataUnit> currBehavUnit_;
	QSharedPointer<StateDataUnit> currStateUnit_;
	qulonglong lastFrameId_;
	QHostAddress ipAddress_;

	QString runningPath_;
private slots:
	void addChangedProperty(QSharedPointer<Property> changedProp);
	void firstPhosphorOperations(double frameTime);
};

/*! @} */

	}; //namespace Engine
}; //namespace Picto

#endif
