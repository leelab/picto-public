#include "PCMMixingSample.h"
#include "../memleakdetect.h"

namespace Picto {

PCMMixingSample::PCMMixingSample()
{
	command_ = PCMMixingSample::NONE;
}

QString PCMMixingSample::getTypeName()
{
	return "PCM";
}

void PCMMixingSample::setSoundEffect(QSharedPointer<PreloadedSound> soundEffect)
{
	if(sound_)
		lastSound_ = sound_;
	sound_ = soundEffect;
	volume_ = 0;
	if(sound_)
		volume_ = sound_->volume();
}

/*! \copydoc MixingSample::scheduleVolumeChange()
 *	\details In the PCMMixingSample scheduled commands are implemented in the update() function.
 */
void PCMMixingSample::scheduleVolumeChange(float volume)
{
	volume_ = volume;
}
/*! \copydoc MixingSample::scheduleStart()
 *	\details In the PCMMixingSample scheduled commands are implemented in the update() function.
 */
void PCMMixingSample::scheduleStart()
{
	command_ = PCMMixingSample::SHOULD_START;
}
/*! \copydoc MixingSample::scheduleStop()
 *	\details In the PCMMixingSample scheduled commands are implemented in the update() function.
 */
void PCMMixingSample::scheduleStop()
{
	command_ = PCMMixingSample::SHOULD_STOP;
}

/*! \brief Implements the effects of the commands scheduled in scheduleStart(), scheduleStop() and 
 *	scheduleVolumeChange().
 * \details When called, this function calls the play(), stop(), setVolue(), etc. functions on the 
 *	underlying PreloadedSound object.  This assures us that all changes to the sound occur synchronously
 *	at a time defined by the experimental presentation system rather than at the particular time that
 *	a user script makes a change.  At the time that the PreloadedSound is stopped or started, this
 *	function emits either a stopping() or playing() signal including the type of this mixing sample
 *	in order to provide feedback to other parties interested in the current state of the underlying
 *	PreloadedSound object.
 *	\note If this object's underlying PreloadedSound changes, calling update will stop the old sound and start
 *	the new one.
 *	\sa PreloadedSound
 */
void PCMMixingSample::update()
{
	if(!sound_)
		return;
	
	//If the sound just changed, stop the old one
	if(lastSound_)
	{
		lastSound_->stop();
		lastSound_.clear();
		playing_ = false;

	}

	if(command_ == SHOULD_STOP)
	{
		sound_->stop();
		command_ = NONE;
		playing_ = false;
		emit stopping(getTypeName());
		return;
	}
	if(sound_->volume() != volume_)
		sound_->setVolume(volume_);
	if(command_ == SHOULD_START)
	{
		sound_->play();
		command_ = NONE;
		playing_ = true;
		emit playing(getTypeName());
	}
	if(playing_ && !sound_->playing())
	{
		playing_ = false;
		emit stopping(getTypeName());
	}
}

}; //namespace Picto
