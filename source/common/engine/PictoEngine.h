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
#include "../storage/BehavioralDataUnit.h"
#include "../storage/StateDataUnit.h"
#include "../storage/RewardDataUnit.h"
#include "../storage/experimentconfig.h"
#include "propertytable.h"

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
	enum {NoCommand, PlayEngine, StopEngine, PauseEngine};
	void play() { engineCommand_ = PlayEngine; };
	void pause() { engineCommand_ = PauseEngine; };
	void stop();
	int getEngineCommand();

	QList<QSharedPointer<RenderingTarget> > getRenderingTargets();
	void addRenderingTarget(QSharedPointer<RenderingTarget> target);
	bool hasVisibleRenderingTargets();

	QSharedPointer<SignalChannel> getSignalChannel(QString name);
	void addSignalChannel(QString name, QSharedPointer<SignalChannel> channel);
	void startAllSignalChannels();
	void stopAllSignalChannels();

	void setEventCodeGenerator(QSharedPointer<EventCodeGenerator> eventCodeGenerator) { eventCodeGenerator_ = eventCodeGenerator; };
	void generateEvent(unsigned int eventCode);

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

	void setLastFrame(qulonglong frameId){lastFrameId_ = frameId;};
	qulonglong getLastFrameId(){return lastFrameId_;};
	QString getLastFrameTime(){return lastFrameTime_;};

	bool updateCurrentStateFromServer();
	void setRunningPath(QString path);
	QString getServerPathUpdate();
	QSharedPointer<Picto::BehavioralDataUnit> getCurrentBehavioralData();

	bool setDataCommandChannel(QSharedPointer<CommandChannel> commandChannel);
	QSharedPointer<CommandChannel> getDataCommandChannel();

	bool setUpdateCommandChannel(QSharedPointer<CommandChannel> commandChannel);
	QSharedPointer<CommandChannel> getUpdateCommandChannel();

	void setPropertyTable(QSharedPointer<PropertyTable> propTable);
	QSharedPointer<PropertyTable> getPropertyTable(){return propTable_;};
	void sendAllPropertyValuesToServer();

	void setSessionId(QUuid sessionId);
	QUuid getSessionId() { return sessionId_; };

	void setName(QString name) { name_ = name; };
	QString getName() { return name_; };

	void setOperatorAsUser(bool operatorMode = true){userIsOperator_ = operatorMode;};
	bool operatorIsUser(){return userIsOperator_;};
	void setSlaveMode(bool mode, CommandChannel *serverChan) { slave_ = mode; slaveCommandChannel_ = serverChan; };
	bool slaveMode() { return slave_; }
	void setLastTimePropertiesRequested(QString time){lastTimePropChangesRequested_ = time;};
	void resetLastTimeStateDataRequested(){lastTimeStateDataRequested_ = "0.0";runningPath_="";currStateUnit_.clear();firstCurrStateUpdate_ = true;};
	void setExperimentConfig(QSharedPointer<ExperimentConfig> expConfig){expConfig_ = expConfig;currStateUnit_.clear();};
	QSharedPointer<ExperimentConfig> getExperimentConfig(){return expConfig_;};
private:
	//QSharedPointer<Experiment> experiment_;
	PictoEngineTimingType::PictoEngineTimingType timingType_;
	bool bExclusiveMode_;
	
	QSharedPointer<EventCodeGenerator> eventCodeGenerator_;
	QMap<QString, QSharedPointer<SignalChannel> > signalChannels_;
	QList<QSharedPointer<RenderingTarget> > renderingTargets_;
	QSharedPointer<RewardController> rewardController_;
	QSharedPointer<PropertyDataUnitPackage> propPackage_;
	QSharedPointer<StateDataUnitPackage> stateDataPackage_;
	QSharedPointer<CommandChannel> dataCommandChannel_;		//Used for sending data to the server
	QSharedPointer<CommandChannel> updateCommandChannel_;	//Used for sending everything except data
	CommandChannel *slaveCommandChannel_;	//Used for communicating with the server in slave mode
	QSharedPointer<PropertyTable> propTable_;
	QList<QSharedPointer<RewardDataUnit>> deliveredRewards_;
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
	QSharedPointer<BehavioralDataUnit> currBehavUnit_;
	QSharedPointer<StateDataUnit> currStateUnit_;
	qulonglong lastFrameId_;
	QString lastFrameTime_;

	QString runningPath_;
private slots:
	void addChangedProperty(QSharedPointer<Property> changedProp);
};

/*! @} */

	}; //namespace Engine
}; //namespace Picto

#endif
