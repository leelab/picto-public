#ifndef _HARDWARE_SETUP_H_
#define _HARDWARE_SETUP_H_

#include <QSharedPointer>

#include "../common/engine/PictoEngine.h"

/*!	\brief Sets up the Picto::Engine::PictoEngine according to the PictoDirector hardware.
 *
 *	\details The PictoDirector could be running on any of a number of hardware platforms.
 *	This tool contains the functions needed to set up the hardware for the
 *	specific platform.  At the moment, all of the code is contained within this 
 *	object.  If we start supporting more than 1 or 2 platforms, it may be worth 
 *	breaking out each configuration seperately.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class HardwareSetup
{
public:
	HardwareSetup(QSharedPointer<Picto::Engine::PictoEngine> engine);

	/*! \brief The types of SignalChannel objects that can be setup.*/
	typedef enum
	{
		Mouse,						//!< Sets up signal input from the Mouse.  No diameter input is used.
		EyetrackerLegacySystemXp,	//!< Sets up signal inputs for a legacy system (Orion)
		EyetrackerPictoBoxXp		//!< Sets up signal inputs for a Pictobox.
	} SignalChannelType;

	/*! \brief The types of VisualTarget objects that can be setup.*/
	typedef enum
	{
		Pixmap,			//!< A Qt Pixmap visual target will be used. (Inprecise timing)
		D3D				//!< A Direct3d full screen visual target will be used.
	} VisualTargetType;

	/*! \brief The types of RewardController objects that can be setup.*/
	typedef enum
	{
		PictoBoxXpReward,		//!< Rewards will be provided from a Pictobox through its rear red-black "speaker" ports.
		LegacySystemXpReward,	//!< Rewards will be provided using a legacy setup (Orion system... note that the reward line will need to be reattached for this.  See LegacySystemXPRewardController.
		AudioReward,			//!< An audio tick will be played for every reward
		NullReward				//!< No Rewards will be supplied
	} RewardControllerType;

	/*! \brief The types of OutputSignalController objects that can be setup.*/
	typedef enum
	{
		PictoBoxXpOutSig,		//!< Output voltage signals are created on the Pictobox BNC ports
		NullOutSig				//!< No output voltage signals will be generated
	} OutputSignalControllerType;

	/*! \brief The types of EventCodeGenerator objects that can be setup.*/
	typedef enum
	{
		PictoBoxXpGen,		//!< Creates event codes on the Pictobox parallel port
		LegacyGen,			//!< Creates event codes in a legacy system (Orion computer)
		NullGen				//!< No event codes will be created
	} EventCodeGeneratorType;

	bool setupRenderingTargets(VisualTargetType visualTargetType, bool timingCritical);
	bool setupSignalChannel(SignalChannelType channelType);
	bool setupRewardController(RewardControllerType controllerType);
	bool setupOutputSignalController(OutputSignalControllerType controllerType, EventCodeGeneratorType generatorType);
	bool setupEventCodeGenerator(EventCodeGeneratorType generatorType);
	/*! \brief Sets the channel numbers used for x and y position inputs.*/
	bool setXYChannelNums(int xChan, int yChan){xChan_ = xChan;yChan_ = yChan;return true;};
	/*! \brief Sets the channel numbers used for x and y diameter inputs.  Negative values indicate that diameter sampling will not be used.*/
	bool setXYDiamChannelNums(int xDiamChan, int yDiamChan){xDiamChan_ = xDiamChan;yDiamChan_ = yDiamChan;return true;};
	/*! \brief Sets the sample periods used for position and diameter sampling in milliseconds.*/
	bool setSamplePeriods(int positionPeriodMs, int diamPeriodMs){posSampPer_ = positionPeriodMs;diamSampPer_ = diamPeriodMs;return true;};

	bool isSetup();

private:
	QSharedPointer<Picto::Engine::PictoEngine> engine_;

	bool renderingTargetsSetup_;
	bool signalChannelSetup_;
	bool rewardControllerSetup_;
	bool outSigControllerSetup_;
	bool eventCodeGenSetup_;
	int xChan_;
	int yChan_;
	int xDiamChan_;
	int yDiamChan_;
	int posSampPer_;
	int diamSampPer_;
};

#endif