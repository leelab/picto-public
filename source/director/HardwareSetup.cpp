#include <QApplication>
#include "HardwareSetup.h"

#define NI_STUFF

#include "../common/controlelements/TestController.h"
#include "../common/controlelements/StopwatchController.h"
#include "../common/controlelements/TargetController.h"
#include "../common/controlelements/ChoiceController.h"
#include "../common/controlelements/FixationController.h"

#include "../common/parameter/BooleanParameter.h"
#include "../common/parameter/NumericParameter.h"
#include "../common/parameter/RangeParameter.h"

#include "../common/compositor/RenderingTarget.h"
#include "../common/compositor/PixmapVisualTarget.h"
#ifdef WIN32
#include "compositor/D3DVisualTarget.h"
#endif
#include "../common/compositor/PCMAuralTarget.h"


#include "../common/engine/XYSignalChannel.h"
#include "../common/engine/GenericInput.h"
#include "../common/engine/MouseInputPort.h"
#if defined WIN32 && defined NI_STUFF
#include "engine/PictoBoxXPAnalogInputPort.h"
#include "engine/LegacySystemXPAnalogInputPort.h"
#endif

#include "../common/iodevices/RewardController.h"
#include "../common/iodevices/OutputSignalController.h"
#include "../common/iodevices/EventCodeGenerator.h"
#include "../common/iodevices/NullRewardController.h"
#include "../common/iodevices/AudioRewardController.h"
#include "../common/iodevices/NullEventCodeGenerator.h"
#include "../common/stimuli/PreloadedSound.h"
#if defined WIN32 && defined NI_STUFF
#include "iodevices/PictoBoxXPRewardController.h"
#include "iodevices/PictoBoxXPOutputSignalController.h"
#include "iodevices/LegacySystemXPRewardController.h"
#include "iodevices/LegacySystemXPEventCodeGenerator.h"
#include "iodevices/PictoBoxXPEventCodeGenerator.h"
#include "iodevices/WinSound.h"
#endif
#include "../common/memleakdetect.h"

/*! \brief Constructs a HardwareSetup object that configures the input Picto::Engine::PictoEngine.
*/
HardwareSetup::HardwareSetup(QSharedPointer<Picto::Engine::PictoEngine> engine)
{
	engine_ = engine;

	renderingTargetsSetup_ = false;
	signalChannelSetup_ = false;
	rewardControllerSetup_ = false;
	eventCodeGenSetup_ = false;
	outSigControllerSetup_ = false;
	xChan_ = 0;
	yChan_ = 1;
	posSampPer_ = 2;
	diamSampPer_ = 4;
}

/*! \brief Returns true if everything has been successfully setup.
*/
bool HardwareSetup::isSetup()
{
	return renderingTargetsSetup_ & 
		signalChannelSetup_ & rewardControllerSetup_ &
		eventCodeGenSetup_ & outSigControllerSetup_;
}


/*!	\brief Sets up the rendering target used in the experiment with appropriate 
 *	Picto::VisualTarget and Picto::PCMAuralTarget objects, then adds it to the Picto::Engine::PictoEngine.
 *
 *	@param visualTargetType The type of VisualTarget to be setup.
 *	@param timingCritical Indicates that this process/thread should run at maximum
 *	priority.  This makes the mouse unresponsive, which is okay 
 *	when input is from the eye tracker.
 *	\details Returns false if the RenderingTarget can not be set up properly.
 */
bool HardwareSetup::setupRenderingTargets(VisualTargetType visualTargetType, bool timingCritical)
{
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	QSharedPointer<Picto::VisualTarget> visualTarget;
	//Tell objects to use WinSound objects for sounds.
	Picto::PreloadedSound::setSoundConstructor(Picto::WinSound::createWinSound);

	if(visualTargetType == Pixmap)
	{
		visualTarget = QSharedPointer<Picto::PixmapVisualTarget>(new Picto::PixmapVisualTarget());
		visualTarget->show();
	}
	if(visualTargetType == D3D)
	{
#ifndef WIN32
		return false;
#else
		visualTarget = QSharedPointer<Picto::D3DVisualTarget>(new Picto::D3DVisualTarget(timingCritical));
		visualTarget->show();
#endif
	}

	QSharedPointer<Picto::RenderingTarget> renderingTarget(new Picto::RenderingTarget(visualTarget, pcmAuralTarget));
	engine_->addRenderingTarget(renderingTarget);

	renderingTargetsSetup_ = true;
	return true;

}

/*!	\brief Sets up the signal channels used for input
 *
 *	Every director instance requires some form of input (mouse, eye tracker, or something
 *	else.  This function sets that up.  The SignalChannelType is used to determine
 *	the type of channel we wish to set up.  The legal choices are:
 *		mouse
 *		EyetrackerLegacySystemXp
 *		EyetrackerPictoBoxXp
 *	Additional choices will be added as we expand platforms.
 *
 *	Returns false if we try to set up signal channels for the wrong platform, or we can't setup
 *	the SignalChannels for some other reason.  Returns true otherwise.
 *	\warning The Mouse Signal channel can't be set up unless the rendering targets have already
 *	been setup.  This is because the MouseSignalChannel requires a visual target.
 *	Note that if we try to set up a signal channel for the wrong platform, we simply return false
 *
 */
bool HardwareSetup::setupSignalChannel(SignalChannelType channelType)
{ 
	if(channelType == Mouse)
	{
		if(!renderingTargetsSetup_)
			return false;

		QSharedPointer<Picto::VisualTarget> visualTarget = engine_->getRenderingTargets().first()->getVisualTarget();
		QSharedPointer<Picto::MouseInputPort> mousePort(new Picto::MouseInputPort(visualTarget.data()));
		QSharedPointer<Picto::XYSignalChannel> mouseChannel(new Picto::XYSignalChannel("Position",0,1,posSampPer_,mousePort));
		engine_->addSignalChannel(mouseChannel);
#ifdef DEVELOPMENTBUILD
		if((xDiamChan_ >= 0) || (yDiamChan_ >=0))
		{
			//Setup DiameterChannel
			QSharedPointer<Picto::XYSignalChannel> daimMouseChannel(new Picto::XYSignalChannel("Diameter",2,3,diamSampPer_,mousePort));
			engine_->addSignalChannel(daimMouseChannel);
		}
#endif
	}
	else if(channelType == EyetrackerLegacySystemXp)
	{
		QApplication::setOverrideCursor(Qt::BlankCursor);	//In case mouse happens to be on monkey screen
#if defined WIN32 && defined NI_STUFF
		//Setup PositionChannel
		QSharedPointer<Picto::LegacySystemXPAnalogInputPort> daqPort(new Picto::LegacySystemXPAnalogInputPort());
		QSharedPointer<Picto::XYSignalChannel> aiChannel(new Picto::XYSignalChannel("Position",xChan_,yChan_,posSampPer_,daqPort));
		engine_->addSignalChannel(aiChannel);

		//Track the used ports - do not make them available as Generic Inputs
		QVector<int> qvUsedPorts;
		qvUsedPorts.push_back(xChan_);
		qvUsedPorts.push_back(yChan_);

		if((xDiamChan_ >= 0) || (yDiamChan_ >=0))
		{
			//Setup DiameterChannel
			QSharedPointer<Picto::XYSignalChannel> aiChannel(new Picto::XYSignalChannel("Diameter",xDiamChan_,yDiamChan_,diamSampPer_,daqPort));
			engine_->addSignalChannel(aiChannel);

			qvUsedPorts.push_back(xDiamChan_);
			qvUsedPorts.push_back(yDiamChan_);
		}

		QSharedPointer<Picto::GenericInput> genericInputs(new Picto::GenericInput(qvUsedPorts, "GenericInputs", daqPort));
		engine_->addSignalChannel(genericInputs);

#else
		return false;
#endif
	}
	else if(channelType == EyetrackerPictoBoxXp)
	{
		QApplication::setOverrideCursor(Qt::BlankCursor);	//In case mouse happens to be on monkey screen
#if defined WIN32 && defined NI_STUFF
		//Setup PositionChannel
		QSharedPointer<Picto::PictoBoxXPAnalogInputPort> daqPort(new Picto::PictoBoxXPAnalogInputPort());
		QSharedPointer<Picto::XYSignalChannel> aiChannel(new Picto::XYSignalChannel("Position",xChan_,yChan_,posSampPer_,daqPort));
		engine_->addSignalChannel(aiChannel);

		//Track the used ports - do not make them available as Generic Inputs
		QVector<int> qvUsedPorts;
		qvUsedPorts.push_back(xChan_);
		qvUsedPorts.push_back(yChan_);

		if((xDiamChan_ >= 0) || (yDiamChan_ >=0))
		{
			//Setup DiameterChannel
			QSharedPointer<Picto::XYSignalChannel> aiChannel(new Picto::XYSignalChannel("Diameter",xDiamChan_,yDiamChan_,diamSampPer_,daqPort));
			engine_->addSignalChannel(aiChannel);

			qvUsedPorts.push_back(xDiamChan_);
			qvUsedPorts.push_back(yDiamChan_);
		}

		QSharedPointer<Picto::GenericInput> genericInputs(new Picto::GenericInput(qvUsedPorts, "GenericInputs", daqPort));
		engine_->addSignalChannel(genericInputs);

#else
		return false;
#endif
	}

	signalChannelSetup_ = true;

	return true;
}

/*!	\brief Sets up the reward controller
 *
 *	\details Potential RewardController are PictoBoxXpReward, LegacySystemXpReward, AudioReward, NullReward.
 *	Returns true on success, false otherwise.
 */
bool HardwareSetup::setupRewardController(RewardControllerType controllerType)
{
	QSharedPointer<Picto::RewardController> rewardController;

	if(controllerType == PictoBoxXpReward)
	{
#if defined WIN32 && defined NI_STUFF
		rewardController = QSharedPointer<Picto::RewardController>(new Picto::PictoBoxXPRewardController());
#else
		return false;
#endif
	}
	else if(controllerType == LegacySystemXpReward)
	{
		rewardController = QSharedPointer<Picto::RewardController>(new Picto::LegacySystemXPRewardController(1));
	}
	else if(controllerType == NullReward)
	{
		rewardController = QSharedPointer<Picto::RewardController>(new Picto::NullRewardController());
	}
	else if(controllerType == AudioReward)
	{
		rewardController = QSharedPointer<Picto::RewardController>(new Picto::AudioRewardController());
	}

	engine_->setRewardController(rewardController);

	rewardControllerSetup_ = true;

	return true;
}

/*!	\brief Sets up the OutputSignalController used to handle output voltage signals.
 *
 *	\details There are only two types of OutputSignalControllers currently: PictoBoxXpOutSig and NullOutSig.
 *	Returns true on success, returns false otherwise.
 *	\note This function needs to know the EventCodeGeneratorType so that it knows if the port used for Event codes is 
 *	free to be used as an OutputSignal port or not.
 */
bool HardwareSetup::setupOutputSignalController(OutputSignalControllerType controllerType, EventCodeGeneratorType generatorType)
{
	QHash<QString,QSharedPointer<Picto::OutputSignalController>> outSigControllers;

	if(controllerType == PictoBoxXpOutSig)
	{
#if defined WIN32 && defined NI_STUFF
		outSigControllers["BNC0"] = QSharedPointer<Picto::PictoBoxXPOutputSignalController>(new Picto::PictoBoxXPOutputSignalController(0));
		if(generatorType == NullGen)
			outSigControllers["PAR0"] = QSharedPointer<Picto::PictoBoxXPOutputSignalController>(new Picto::PictoBoxXPOutputSignalController(1));
#else
		return false;
#endif
	}
	else if(controllerType == NullOutSig)
	{
		outSigControllers["NULL"] = QSharedPointer<Picto::OutputSignalController>();
	}

	for(QHash<QString,QSharedPointer<Picto::OutputSignalController>>::iterator it = outSigControllers.begin();
		it != outSigControllers.end();it++)
	{
		engine_->setOutputSignalController(it.key(),it.value());
	}
	

	outSigControllerSetup_ = true;

	return true;
}

/*!	\brief Sets up the EventCodeGenerator used to send event codes to a neural data acquisition system.
 *
 *	\details Options are PictoBoxXpGen, LegacyGen, NullGen.
 *	Returns true on success, false otherwise.
 */
bool HardwareSetup::setupEventCodeGenerator(HardwareSetup::EventCodeGeneratorType generatorType)
{
	QSharedPointer<Picto::EventCodeGenerator> generator;

	if(generatorType == PictoBoxXpGen)
	{
#if defined WIN32 && defined NI_STUFF
		generator = QSharedPointer<Picto::EventCodeGenerator>(new Picto::PictoBoxXPEventCodeGenerator());
#else
		return false;
#endif
	}
	else if(generatorType == LegacyGen)
	{
		generator = QSharedPointer<Picto::EventCodeGenerator>(new Picto::LegacySystemXPEventCodeGenerator());
	}
	else if(generatorType == NullGen)
	{
		generator = QSharedPointer<Picto::EventCodeGenerator>(new Picto::NullEventCodeGenerator());
	}

	engine_->setEventCodeGenerator(generator);

	eventCodeGenSetup_ = true;

	return true;
}