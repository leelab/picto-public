#include <QApplication>
#include "HardwareSetup.h"

#define NI_STUFF

/*#include "../common/stimuli/VisualElementFactory.h"
#include "../common/stimuli/ArrowGraphic.h"
#include "../common/stimuli/BoxGraphic.h"
#include "../common/stimuli/CircleGraphic.h"
#include "../common/stimuli/EllipseGraphic.h"
#include "../common/stimuli/LineGraphic.h"
#include "../common/stimuli/PictureGraphic.h"
#include "../common/stimuli/RandomlyFilledGridGraphic.h"
#include "../common/stimuli/TextGraphic.h"*/

#include "../common/controlelements/ControlElementFactory.h"
#include "../common/controlelements/TestController.h"
#include "../common/controlelements/StopwatchController.h"
#include "../common/controlelements/TargetController.h"
#include "../common/controlelements/ChoiceController.h"

#include "../common/parameter/ParameterFactory.h"
#include "../common/parameter/BooleanParameter.h"
#include "../common/parameter/ChoiceParameter.h"
#include "../common/parameter/NumericParameter.h"
#include "../common/parameter/RangeParameter.h"

#include "../common/compositor/RenderingTarget.h"
#include "../common/compositor/PixmapVisualTarget.h"
#ifdef WIN32
#include "compositor/D3DVisualTarget.h"
#endif
#include "../common/compositor/PCMAuralTarget.h"


#include "../common/engine/XYSignalChannel.h"
#include "../common/engine/MouseInputPort.h"
#if defined WIN32 && defined NI_STUFF
#include "engine/PictoBoxXPAnalogInputPort.h"
#include "engine/LegacySystemXPAnalogInputPort.h"
#endif

#include "../common/iodevices/RewardController.h"
#include "../common/iodevices/EventCodeGenerator.h"
#include "../common/iodevices/NullRewardController.h"
#include "../common/iodevices/AudioRewardController.h"
#include "../common/iodevices/NullEventCodeGenerator.h"
#if defined WIN32 && defined NI_STUFF
#include "iodevices/PictoBoxXPRewardController.h"
#include "iodevices/LegacySystemXPRewardController.h"
#include "iodevices/LegacySystemXPEventCodeGenerator.h"
#include "iodevices/PictoBoxXPEventCodeGenerator.h"
#endif
#include "../common/memleakdetect.h"

HardwareSetup::HardwareSetup(QSharedPointer<Picto::Engine::PictoEngine> engine)
{
	engine_ = engine;

	renderingTargetsSetup_ = false;
	signalChannelSetup_ = false;
	rewardControllerSetup_ = false;
	eventCodeGenSetup_ = false;
	xChan_ = 0;
	yChan_ = 1;
}

//! Returns true if everything has been setup
bool HardwareSetup::isSetup()
{
	return renderingTargetsSetup_ & 
		signalChannelSetup_ & rewardControllerSetup_ &
		eventCodeGenSetup_;
}


/*!	\brief	Sets up the rendering target used in the experiment
 *
 *	As this is an early release, we don't set up the audio target.
 *
 *	Note that if the visual target is of a type not supported by the 
 *	platform, we return false
 *
 *	timingCritical indicates that this process/thread should run at maximum
 *	priority.  This makes the mouse unresponsive, which is okay 
 *	when input is from the eye tracker.
 */
bool HardwareSetup::setupRenderingTargets(VisualTargetType visualTargetType, bool timingCritical)
{
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	QSharedPointer<Picto::VisualTarget> visualTarget;

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

/*!	\brief Sets up the signal channel used for input
 *
 *	Every director instance requires some form of input (mouse, eye tracker, or something
 *	else.  This function sets that up.  The channelType string is used to determine
 *	the type of channel we wish to set up.  The legal choices are:
 *		mouse
 *		EyetrackerLegacySystemXp
 *		EyetrackerPictoBoxXp
 *	Additional choice will be added as we expand platforms (actually, the CE stuff would be
 *	really easy to add, but I don't have time to test it...)
 *
 *	Note that if we try to set up a signal channel for the wrong platform, we simply return false
 *
 *	IMPORTANT: The Mouse Signal channel can't be set up unless the rendering targets have already
 *	been setup.  This is because the MouseSignalChannel requires a visual target.
 */
bool HardwareSetup::setupSignalChannel(SignalChannelType channelType)
{ 
	if(channelType == Mouse)
	{
		if(!renderingTargetsSetup_)
			return false;

		QSharedPointer<Picto::VisualTarget> visualTarget = engine_->getRenderingTargets().first()->getVisualTarget();
		QSharedPointer<Picto::MouseInputPort> mousePort(new Picto::MouseInputPort(visualTarget.data()));
		QSharedPointer<Picto::XYSignalChannel> mouseChannel(new Picto::XYSignalChannel("Position",0,1,2,mousePort));
		engine_->addSignalChannel(mouseChannel);
#ifdef DEVELOPMENTBUILD
		if((xDiamChan_ >= 0) || (yDiamChan_ >=0))
		{
			//Setup DiameterChannel
			QSharedPointer<Picto::XYSignalChannel> daimMouseChannel(new Picto::XYSignalChannel("Diameter",2,3,4,mousePort));
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
		QSharedPointer<Picto::XYSignalChannel> aiChannel(new Picto::XYSignalChannel("Position",xChan_,yChan_,2,daqPort));
		engine_->addSignalChannel(aiChannel);

		if((xDiamChan_ >= 0) || (yDiamChan_ >=0))
		{
			//Setup DiameterChannel
			QSharedPointer<Picto::XYSignalChannel> aiChannel(new Picto::XYSignalChannel("Diameter",xDiamChan_,yDiamChan_,4,daqPort));
			engine_->addSignalChannel(aiChannel);
		}

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
		QSharedPointer<Picto::XYSignalChannel> aiChannel(new Picto::XYSignalChannel("Position",xChan_,yChan_,2,daqPort));
		engine_->addSignalChannel(aiChannel);

		if((xDiamChan_ >= 0) || (yDiamChan_ >=0))
		{
			//Setup DiameterChannel
			QSharedPointer<Picto::XYSignalChannel> aiChannel(new Picto::XYSignalChannel("Diameter",xDiamChan_,yDiamChan_,4,daqPort));
			engine_->addSignalChannel(aiChannel);
		}
#else
		return false;
#endif
	}

	signalChannelSetup_ = true;

	return true;
}

/*!	\brief Sets up the reward controller
 *
 *	Initially there are only 2 possibile reward controllers: The PictoBoxXP
 *	controller, and the null controller.
 */
bool HardwareSetup::setupRewardController(RewardControllerType controllerType)
{
	QSharedPointer<Picto::RewardController> rewardController;

	if(controllerType == PictoBoxXpReward)
	{
#if defined WIN32 && defined NI_STUFF
		rewardController = QSharedPointer<Picto::RewardController>(new Picto::PictoBoxXPRewardController(1));
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

/*!	\brief Sets up the event code generator
 *
 *	Initially there are only 2 possibile event code generator: The PictoBoxXP
 *	generator, and the null generator.
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