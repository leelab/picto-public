#include "HardwareSetup.h"


#include "../common/stimuli/VisualElementFactory.h"
#include "../common/stimuli/ArrowGraphic.h"
#include "../common/stimuli/BoxGraphic.h"
#include "../common/stimuli/CircleGraphic.h"
#include "../common/stimuli/EllipseGraphic.h"
#include "../common/stimuli/LineGraphic.h"
#include "../common/stimuli/PictureGraphic.h"
#include "../common/stimuli/RandomlyFilledGridGraphic.h"

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


#include "../common/engine/MouseSignalChannel.h"
#ifdef WIN32
#include "engine/PictoBoxXPAnalogInputSignalChannel.h"
#endif

#include "../common/iodevices/RewardController.h"
#include "../common/iodevices/EventCodeGenerator.h"
#include "../common/iodevices/NullRewardController.h"
#include "../common/iodevices/NullEventCodeGenerator.h"
#ifdef WIN32
#include "iodevices/PictoBoxXPRewardController.h"
#include "iodevices/PictoBoxXPEventCodeGenerator.h"
#endif


HardwareSetup::HardwareSetup(QSharedPointer<Picto::Engine::PictoEngine> engine)
{
	engine_ = engine;

	factoriesSetup_ = false;
	renderingTargetsSetup_ = false;
	signalChannelSetup_ = false;
	rewardControllerSetup_ = false;
	eventCodeGenSetup_ = false;
}

//! Returns true if everything has been setup
bool HardwareSetup::isSetup()
{
	return factoriesSetup_ & renderingTargetsSetup_ & 
		signalChannelSetup_ & rewardControllerSetup_ &
		eventCodeGenSetup_;
}

/*! \brief Sets up all of the factories used by the Director
 *
 *	There are a number of factories used by Director that need to be set up prior
 *	to usage.  This functions sets all of them up.  If there are changes in the
 *	factories, or if new elements are added (e.g. additional VisualStimuli objects)
 *	this function will need to be modified).
 */
bool HardwareSetup::setupFactories()
{
	//Set up the VisualElementFactory
	Picto::VisualElementFactory visualElementFactory;
	visualElementFactory.addVisualElementType(Picto::ArrowGraphic::type, &Picto::ArrowGraphic::NewVisualElement);
	visualElementFactory.addVisualElementType(Picto::BoxGraphic::type, &Picto::BoxGraphic::NewVisualElement);
	visualElementFactory.addVisualElementType(Picto::CircleGraphic::type, &Picto::CircleGraphic::NewVisualElement);
	visualElementFactory.addVisualElementType(Picto::EllipseGraphic::type, &Picto::EllipseGraphic::NewVisualElement);
	visualElementFactory.addVisualElementType(Picto::LineGraphic::type, &Picto::LineGraphic::NewVisualElement);
	visualElementFactory.addVisualElementType(Picto::PictureGraphic::type, &Picto::PictureGraphic::NewVisualElement);
	visualElementFactory.addVisualElementType(Picto::RandomlyFilledGridGraphic::type, &Picto::RandomlyFilledGridGraphic::NewVisualElement);


	//Set up the ControlElementFactory
	Picto::ControlElementFactory controlElementFactory;
	controlElementFactory.addControlElementType(Picto::TestController::ControllerType(), &Picto::TestController::NewTestController);
	controlElementFactory.addControlElementType(Picto::StopwatchController::ControllerType(), &Picto::StopwatchController::NewStopwatchController);
	controlElementFactory.addControlElementType(Picto::TargetController::ControllerType(), &Picto::TargetController::NewTargetController);
	controlElementFactory.addControlElementType(Picto::ChoiceController::ControllerType(), &Picto::ChoiceController::NewChoiceController);

	Picto::ParameterFactory parameterFactory;
	parameterFactory.addParameterType("Boolean",&Picto::BooleanParameter::NewParameter);
	parameterFactory.addParameterType("Choice",&Picto::ChoiceParameter::NewParameter);
	parameterFactory.addParameterType("Numeric",&Picto::NumericParameter::NewParameter);
	parameterFactory.addParameterType("Range",&Picto::RangeParameter::NewParameter);

	factoriesSetup_ = true;

	return true;
}

/*!	\brief	Sets up the rendering targest used in the experiment
 *
 *	As this is an early release, we don't set up the audio target.
 *
 *	Note that if the visual target is of a type not supported by the 
 *	platform, we return false
 */
bool HardwareSetup::setupRenderingTargets(VisualTargetType visualTargetType)
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
		visualTarget = QSharedPointer<Picto::D3DVisualTarget>(new Picto::D3DVisualTarget());
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
 *		eyetrackerXP
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
		QSharedPointer<Picto::MouseSignalChannel> mouseChannel(new Picto::MouseSignalChannel(10,visualTarget));
		engine_->addSignalChannel("PositionChannel",mouseChannel);
	}
	else if(channelType == EyetrackerXp)
	{
#ifndef WIN32
		return false;
#else
		QSharedPointer<Picto::PictoBoxXPAnalogInputSignalChannel> aiChannel(new Picto::PictoBoxXPAnalogInputSignalChannel(250));
		aiChannel->addAiChannel("xpos",1);
		aiChannel->addAiChannel("ypos",0);
		engine_->addSignalChannel("PositionChannel",aiChannel);
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
#ifndef WIN32
		return false;
#else
		rewardController = QSharedPointer<Picto::RewardController>(new Picto::PictoBoxXPRewardController(1));
		rewardController->setRewardResetTimeMs(1,500);
#endif
	}
	else if(controllerType == NullReward)
	{
		rewardController = QSharedPointer<Picto::RewardController>(new Picto::NullRewardController());
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
#ifndef WIN32
		return false;
#else
		generator = QSharedPointer<Picto::EventCodeGenerator>(new Picto::PictoBoxXPEventCodeGenerator());
#endif
	}
	else if(generatorType == NullGen)
	{
		generator = QSharedPointer<Picto::EventCodeGenerator>(new Picto::NullEventCodeGenerator());
	}

	engine_->setEventCodeGenerator(generator);

	eventCodeGenSetup_ = true;

	return true;
}