#ifndef _HARDWARE_SETUP_H_
#define _HARDWARE_SETUP_H_

#include <QSharedPointer>

#include "../common/engine/PictoEngine.h"

/*!	\brief Sets up the hardware for PictoDirector
 *
 *	pictoDirector could be running on any of a number of hardware platforms.
 *	This tool contains the functions needed to set up the hardware for the
 *	specific platform.  At the moment, all of the code is contained within this 
 *	object.  If we start supporting more than 1 or 2 platforms, it may be worth 
 *	breaking out each configuration seperately
 */

class HardwareSetup
{
public:
	HardwareSetup(QSharedPointer<Picto::Engine::PictoEngine> engine);

	typedef enum
	{
		Mouse,EyetrackerLegacySystemXp,EyetrackerPictoBoxXp
	} SignalChannelType;

	typedef enum
	{
		Pixmap,D3D
	} VisualTargetType;

	typedef enum
	{
		PictoBoxXpReward, LegacySystemXpReward, AudioReward, NullReward
	} RewardControllerType;

	typedef enum
	{
		PictoBoxXpGen, NullGen
	} EventCodeGeneratorType;

	bool setupRenderingTargets(VisualTargetType visualTargetType);
	bool setupSignalChannel(SignalChannelType channelType);
	bool setupRewardController(RewardControllerType controllerType);
	bool setupEventCodeGenerator(EventCodeGeneratorType generatorType);
	bool setXYChannelNums(int xChan, int yChan){xChan_ = xChan;yChan_ = yChan;return true;};

	bool isSetup();

private:
	QSharedPointer<Picto::Engine::PictoEngine> engine_;

	bool renderingTargetsSetup_;
	bool signalChannelSetup_;
	bool rewardControllerSetup_;
	bool eventCodeGenSetup_;
	int xChan_;
	int yChan_;

};

#endif