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

void PCMMixingSample::scheduleVolumeChange(float volume)
{
	volume_ = volume;
}

void PCMMixingSample::scheduleStart()
{
	command_ = PCMMixingSample::SHOULD_START;
}

void PCMMixingSample::scheduleStop()
{
	command_ = PCMMixingSample::SHOULD_STOP;
}

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
