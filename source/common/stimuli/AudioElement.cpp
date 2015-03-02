#include "AudioElement.h"
#include "../parameter/AudioFileParameter.h"
#include "../memleakdetect.h"

namespace Picto {

	const QString AudioElement::type = "Simple";
/*! \brief Constructs an AudioElement.
 *	\details Adds an AudioFile Property for handling the name of the AudioFile element that will be played.  Adds a Playing and Trigger booleans
 *	that are not visible to the designer but used to track the current playback status so that it can be correctly reproduced in playback.
 */
AudioElement::AudioElement()
{
	AddDefinableProperty("AudioFile","");
	AddDefinableProperty(QVariant::Bool,"Playing",0);
	AddDefinableProperty(QVariant::Bool,"Trigger",0);	//Changes every time a command occurs.  Command can be read according to what's in "Playing"
}

/*! \brief Creates a new AudioElement object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> AudioElement::Create()
{
	return QSharedPointer<Asset>(new AudioElement());
}

/*! \brief This function is analogous to the VisualElement::addCompositingSurface() function.  It adds a MixingSample to which this element's audio data
 *	will be copied for mixing.
 *	\details The input mixingSample is added to a lookup table indexed by the MixingSample::getTypeName() value.
 */
void AudioElement::addMixingSample(QSharedPointer<MixingSample> mixingSample)
{
	QSharedPointer<PreloadedSound> sound = getSound();
	if(sound)
	{
		mixingSample->setSoundEffect(sound);
	}
	mixingSamples_[mixingSample->getTypeName()] = mixingSample;
	connect(mixingSample.data(),SIGNAL(playing(QString)),this,SLOT(playOccured(QString)));
	connect(mixingSample.data(),SIGNAL(stopping(QString)),this,SLOT(stopOccured(QString)));
	doSoundCommands();
}

/*! \brief Returns the MixingSample with the input sampleType that is used by this AudioElement.
*/
QSharedPointer<MixingSample> AudioElement::getMixingSample(QString sampleType)
{
	if(mixingSamples_.contains(sampleType))
	{
		return mixingSamples_.value(sampleType);
	}
	else
	{
		return QSharedPointer<MixingSample>();
	}
}

/*! \brief Returns the name of the AudioFile element that contains the audio file used by this AudioElement.*/
QString AudioElement::getFile()
{
	return propertyContainer_->getPropertyValue("AudioFile").toString();	
}
	
/*! \brief Sets the name of the AudioFile element from which this AudioElement will play audio.*/
void AudioElement::setFile(QString fileObjectName)
{
	propertyContainer_->setPropertyValue("AudioFile",fileObjectName);
}
	
/*! \brief Call this function to start the underlying audio file playing at the time of the next frame.
 */
void AudioElement::play()
{
	propertyContainer_->setPropertyValue("Playing",true);
	propertyContainer_->setPropertyValue("Trigger",!(propertyContainer_->getPropertyValue("Trigger").toBool()));
}

/*! \brief Call this function to stop the underlying audio file playback at the time of the next frame.
 */
void AudioElement::stop()
{
	propertyContainer_->setPropertyValue("Playing",false);
	propertyContainer_->setPropertyValue("Trigger",!(propertyContainer_->getPropertyValue("Trigger").toBool()));
}

/*! \brief Returns true if this AudioElement is currently playing its AudioFile.
 */
bool AudioElement::isPlaying()
{
	return propertyContainer_->getPropertyValue("Playing").toBool();
}
/*! \brief Use this to avoid lags in an experiment when an AudioFile is first set to an AudioElement.
 *	\details If an AudioFile is set to an AudioElement using its file ScriptProperty for the first time during the course of an Experiment, 
 *	the element must perform the overhead necessary to create a new underlying 'sound' and this could cause some lag in Experimental execution.  
 *	The system caches the sound from the AudioFileParameter stored in the AudioFile field automatically, but it can't predict what other AudioFileParameter's 
 *	sound it might need to cache.  To avoid this, call cacheFile(fileElementName) with each additional file that can be played back in this AudioElement at 
 *	the beginning of the experiment so that the element can preload all sounds that it will need to play.
 */
void AudioElement::cacheFile(QString fileElementName)
{
	updateFileObject(fileElementName);
}

void AudioElement::postDeserialize()
{
	OutputElement::postDeserialize();
	propertyContainer_->getProperty("Trigger")->setVisible(false);
	propertyContainer_->getProperty("Playing")->setVisible(false);
	propertyContainer_->getProperty("UIEnabled")->setVisible(false);
	connect(propertyContainer_->getProperty("AudioFile").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(audioFileChanged(Property*,QVariant)));
	connect(propertyContainer_->getProperty("Trigger").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(commandChanged(Property*,QVariant)));
}

/*! \brief Extends OutputElement::validateObject() to verify that the value in the AudioFile Property is a valid AudioFileParameter.*/
bool AudioElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!OutputElement::validateObject(xmlStreamReader))
		return false;
	QString audioFileElement = propertyContainer_->getPropertyValue("AudioFile").toString();
	updateFileObject(audioFileElement);
	if(!cachedSounds_.contains(audioFileElement))
	{
		QString errMsg = QString("The Audio File: %1, could not be found.").arg(audioFileElement);
		addError(errMsg);
		return false;
	}
	return true;
}

void AudioElement::scriptableContainerWasReinitialized()
{
	OutputElement::scriptableContainerWasReinitialized();
	//In case the AudioFileParameter's name changed and that's what resulted in the reinitialization, we need to reload the AudioFile using updateFileObject.
	updateFileObject(propertyContainer_->getPropertyValue("AudioFile").toString());
}

/*! \brief Extends OutputElement::executeSearchAlgorithm() to look for the input searchRequest in the AudioFile Parameter.*/
bool AudioElement::executeSearchAlgorithm(SearchRequest searchRequest)
{
	if(OutputElement::executeSearchAlgorithm(searchRequest))
		return true;
	switch(searchRequest.type)
	{
	case SearchRequest::STRING:
		{
			//Search my control target for the string
			QString fileName = propertyContainer_->getPropertyValue("AudioFile").toString();
			if(!fileName.isNull() && fileName.contains(searchRequest.query,searchRequest.caseSensitive?Qt::CaseSensitive:Qt::CaseInsensitive))
				return true;
		}
		break;
	};
	return false;
}

/*! \brief Returns the PreloadedSound object currently associated with this AudioElement.
 *	\details The returned value will come from a cached version of the sound in the AudioFileParameter 
 *	referenced in AudioFile.
 *	\sa cacheFile()
 */
QSharedPointer<PreloadedSound> AudioElement::getSound()
{
	QString targetName = propertyContainer_->getPropertyValue("AudioFile").toString();
	Q_ASSERT(cachedSounds_.contains(targetName));
	return cachedSounds_[targetName];
}

/*! \brief Finds the AudioFileParameter with the input fileElementName, caches a copy of its PreloadedSound and adds
 *	it to all of the MixingSample objects that have been adde to this AudioElement.
 *	\details If an AudioFileParameter cannot be found that has the input name, a runtime error will be triggered.
*/
void AudioElement::updateFileObject(QString fileElementName)
{
	QList<QWeakPointer<Scriptable>> scriptables = getScriptableList();
	if((!fileObj_.isNull()) && fileObj_.toStrongRef()->getName() == fileElementName)
		return;
	foreach(QWeakPointer<Scriptable> scriptable,scriptables)
	{
		if(scriptable.isNull())
			continue;
		if(scriptable.toStrongRef()->getName() == fileElementName && scriptable.toStrongRef()->inherits("Picto::AudioFileParameter"))
		{
			QSharedPointer<PreloadedSound> sound = (scriptable.toStrongRef().staticCast<AudioFileParameter>())->createSound();
			cachedSounds_[fileElementName] = sound;
			foreach(QSharedPointer<MixingSample> mixSamp,mixingSamples_.values())
			{
				mixSamp->setSoundEffect(sound);
			}
			return;
		}
	}
	QScriptContext* ctx = context();
	if(ctx)
		ctx->throwError("Couldn't find file object: " + fileElementName);
}

/*! \brief Performs the latest sound commands that were set with the play() or stop() functions.
 *	\details What actually happens here is that a start or stop is scheduled in all MixingSamples
 *	depending on whether play() or stop() was selected.  We don't simply do this inside those
 *	functions since this indirection allows the same system to trigger playback in a SlaveExperiment.
 *	Since we use a change in the Trigger Property to trigger a call to this function, and this function
 *	just looks at the Playing value.  The same command will occur on both the Master and Slave 
 *	Experiments.
 */
void AudioElement::doSoundCommands()
{
	QSharedPointer<PreloadedSound> sound = getSound();
	if(!sound)
		return;
	bool command = propertyContainer_->getPropertyValue("Playing").toBool();
	if(command)
	{
		foreach(QSharedPointer<MixingSample> mixSamp,mixingSamples_.values())
			mixSamp->scheduleStart();
	}
	else
	{
		foreach(QSharedPointer<MixingSample> mixSamp,mixingSamples_.values())
			mixSamp->scheduleStop();
	}
}

/*! \brief Called when the AudioFile Property changes to make sure that we update our current sound file using updateFileObject().
*/
void AudioElement::audioFileChanged(Property*,QVariant)
{
	updateFileObject(propertyContainer_->getPropertyValue("AudioFile").toString());
}

/*! \brief Called when the Trigger Property changes to make sure that we perform all of the latest sound commans.  See doSoundCommands() for more details.
 *	\details Internally, this just calls doSoundCommands().
 */
void AudioElement::commandChanged(Property*,QVariant)
{
	doSoundCommands();
}

/*! \brief Called when the underlying sound actually starts playing in a MixingSample.  Updates the Playing Property so that the rest of the
 *	StateMachine will have a way to see if the underlying sound is actually playing by checking isPlaying().
 */
void AudioElement::playOccured(QString)
{
	propertyContainer_->setPropertyValue("Playing",true);
}

/*! \brief Called when the underlying sound actually stops playing in a MixingSample.  Updates the Playing Property so that the rest of the
 *	StateMachine will have a way to see if the underlying sound is actually playing by checking isPlaying().
 */
void AudioElement::stopOccured(QString)
{
	propertyContainer_->setPropertyValue("Playing",false);
}

}; //namespace Picto
