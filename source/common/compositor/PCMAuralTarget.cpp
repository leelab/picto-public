#include <QApplication>
#include "PCMAuralTarget.h"
#include "PCMMixingSample.h"
#include "../memleakdetect.h"

namespace Picto {

PCMAuralTarget::PCMAuralTarget()
{
	 activeSoundLookup_ = QSharedPointer<QHash<MixingSample*,QSharedPointer<MixingSample>>>(new QHash<MixingSample*,QSharedPointer<MixingSample>>());
}

QSharedPointer<MixingSample> PCMAuralTarget::generateMixingSample()
{
	QSharedPointer<PCMMixingSample> pcmMixingSample(new PCMMixingSample());

	return pcmMixingSample.staticCast<MixingSample>();
}

QString PCMAuralTarget::getTypeName()
{
	return "PCM";
}

void PCMAuralTarget::mix(QSharedPointer<MixingSample> mixSamp)
{
	soundsToMix_.append(mixSamp);
}


void PCMAuralTarget::present()
{
	//Build up a new sounds lookup table and put it in a buffer
	QSharedPointer<QHash<MixingSample*,QSharedPointer<MixingSample>>> newActiveLookup(new QHash<MixingSample*,QSharedPointer<MixingSample>>());
	foreach(QSharedPointer<MixingSample> sound, soundsToMix_)
	{
		(*newActiveLookup)[sound.data()] = sound;
	}

	//Find all sounds that were active before and are no longer active
	foreach(MixingSample* soundPtr,activeSoundLookup_->keys())
	{
		if(newActiveLookup->contains(soundPtr))
			activeSoundLookup_->remove(soundPtr);
	}
	//All the sounds that were active before and no longer should be set to stop and added to 
	//the update list
	QList<QSharedPointer<MixingSample>> soundsToDeactivate = activeSoundLookup_->values();
	foreach(QSharedPointer<MixingSample> sound,soundsToDeactivate)
	{
		sound->scheduleVolumeChange(0);
		sound->scheduleStop();
	}
	soundsToMix_ << soundsToDeactivate;

	//replace the old active sounds lookup table with the new one in the buffer
	activeSoundLookup_ = newActiveLookup;

	//Go through each "sound to mix" and call update on it
	QSharedPointer<PCMMixingSample> pcmSound;
	foreach(QSharedPointer<MixingSample> sound,soundsToMix_)
	{
		pcmSound = sound.dynamicCast<PCMMixingSample>();
		Q_ASSERT(pcmSound);
		pcmSound->update();
	}
	soundsToMix_.clear();
}


}; //namespace Picto
