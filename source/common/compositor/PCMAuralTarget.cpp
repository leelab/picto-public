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

/*! \copydoc AuralTarget::mix()
 * \details In the case of the PCMAuralTarget, this simply adds the input
 * PCMMixingSample to a list.
 * \sa PCMAuralTarget
 */
void PCMAuralTarget::mix(QSharedPointer<MixingSample> mixSamp)
{
	//To mix the MixingSample into this AuralTarget, we just add it to our SoundsToMix_ list.
	soundsToMix_.append(mixSamp);
}

/*! \copydoc AuralTarget::present()
 * \details In the case of the PCMAuralTarget, we simply set MixingSamples that weren't mixed in
 * this frame to stop and then call update on all of these as well as the MixingSamples that were
 * mixed in.
 * \sa PCMAuralTarget
 */
void PCMAuralTarget::present()
{
	//Build up a new sounds lookup table and put it in a buffer
	QSharedPointer<QHash<MixingSample*,QSharedPointer<MixingSample>>> newActiveLookup(new QHash<MixingSample*,QSharedPointer<MixingSample>>());
	foreach(QSharedPointer<MixingSample> sound, soundsToMix_)
	{
		(*newActiveLookup)[sound.data()] = sound;
	}

	//Find all sounds that were active before and are no longer active
	//by removing all active sounds from the activeSoundLookup_ table
	foreach(MixingSample* soundPtr,activeSoundLookup_->keys())
	{
		if(newActiveLookup->contains(soundPtr))
			activeSoundLookup_->remove(soundPtr);
	}
	//We now retrieve all the sounds that were active but are no longer active by checking
	//the values the just updated activeSoundLookup_ table.  All of these sounds should 
	//be set to stop and added to the update list
	QList<QSharedPointer<MixingSample>> soundsToDeactivate = activeSoundLookup_->values();
	foreach(QSharedPointer<MixingSample> sound,soundsToDeactivate)
	{
		sound->scheduleVolumeChange(0);
		sound->scheduleStop();
	}
	soundsToMix_ << soundsToDeactivate;

	//replace the old active sounds lookup table with the new one in the buffer
	activeSoundLookup_ = newActiveLookup;

	//Go through each "sound to mix" and call update on it.  This results in starting all
	//sounds that weren't playing before and stopping all sounds that were just set to 
	//stop.
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
