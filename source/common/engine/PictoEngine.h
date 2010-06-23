/*! \file PictoEngine.h
 * \ingroup picto_engine
 * \brief
 */

#ifndef _PICTOENGINE_H_
#define _PICTOENGINE_H_

#include "../common.h"
#include "../compositor/RenderingTarget.h"
//#include "../task/Task.h"
//#include "../experiment/Experiment.h"
#include "../network/CommandChannel.h"
#include "../protocol/ProtocolCommand.h"
#include "SignalChannel.h"
#include "../iodevices/RewardController.h"
#include "../iodevices/EventCodeGenerator.h"

#include <QSharedPointer>
#include <QUuid>

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
class PICTOLIB_API PictoEngine
#else
class PictoEngine
#endif
{
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
	enum {NoCommand, ResumeEngine, StopEngine, PauseEngine};
	void resume() { engineCommand_ = ResumeEngine; };
	void pause() { engineCommand_ = PauseEngine; };
	void stop();
	void clearEngineCommand() {engineCommand_ = NoCommand; };
	int getEngineCommand();

	QList<QSharedPointer<RenderingTarget> > getRenderingTargets();
	void addRenderingTarget(QSharedPointer<RenderingTarget> target);

	QSharedPointer<SignalChannel> getSignalChannel(QString name);
	void addSignalChannel(QString name, QSharedPointer<SignalChannel> channel);
	void startAllSignalChannels();
	void stopAllSignalChannels();

	void setEventCodeGenerator(QSharedPointer<EventCodeGenerator> eventCodeGenerator) { eventCodeGenerator_ = eventCodeGenerator; };
	void generateEvent(unsigned int eventCode);

	void setRewardController(QSharedPointer<RewardController> rewardController) { rewardController_ = rewardController; };
	void giveReward(int channel);

	bool setDataCommandChannel(QSharedPointer<CommandChannel> commandChannel);
	QSharedPointer<CommandChannel> getDataCommandChannel();

	bool setUpdateCommandChannel(QSharedPointer<CommandChannel> commandChannel);
	QSharedPointer<CommandChannel> getUpdateCommandChannel();

	void setSessionId(QUuid sessionId);
	QUuid getSessionId() { return sessionId_; };

	void setName(QString name) { name_ = name; };
	QString getName() { return name_; };

	void setSlaveMode(bool mode, CommandChannel *serverChan) { slave_ = mode; slaveCommandChannel_ = serverChan; };
	bool slaveMode() { return slave_; }
	CommandChannel* getSlaveCommandChannel() { return slaveCommandChannel_; };

private:
	//QSharedPointer<Experiment> experiment_;
	PictoEngineTimingType::PictoEngineTimingType timingType_;
	bool bExclusiveMode_;
	
	QSharedPointer<EventCodeGenerator> eventCodeGenerator_;
	QMap<QString, QSharedPointer<SignalChannel> > signalChannels_;
	QList<QSharedPointer<RenderingTarget> > renderingTargets_;
	QSharedPointer<RewardController> rewardController_;
	QSharedPointer<CommandChannel> dataCommandChannel_;		//Used for sending data to the server
	QSharedPointer<CommandChannel> updateCommandChannel_;	//Used for sending everything except data
	CommandChannel *slaveCommandChannel_;	//Used for communicating with the server in slave mode

	QUuid sessionId_;
	QString name_;

	int engineCommand_;

	bool slave_;
};

/*! @} */

	}; //namespace Engine
}; //namespace Picto

#endif
