#include "AudioElement.h"
#include "../parameter/AudioFileParameter.h"
#include "../memleakdetect.h"

namespace Picto {

AudioElement::AudioElement()
{
	AddDefinableProperty("AudioFile","");
	AddDefinableProperty(QVariant::Bool,"Playing",0);
	AddDefinableProperty(QVariant::Bool,"Trigger",0);	//Changes every time a command occurs.  Command can be read according to what's in "Playing"
}

QSharedPointer<Asset> AudioElement::Create()
{
	return QSharedPointer<Asset>(new AudioElement());
}

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

QString AudioElement::getFile()
{
	return propertyContainer_->getPropertyValue("AudioFile").toString();	
}
	
void AudioElement::setFile(QString fileObjectName)
{
	propertyContainer_->setPropertyValue("AudioFile",fileObjectName);
}
	
void AudioElement::play()
{
	propertyContainer_->setPropertyValue("Playing",true);
	propertyContainer_->setPropertyValue("Trigger",!(propertyContainer_->getPropertyValue("Trigger").toBool()));
}

void AudioElement::stop()
{
	propertyContainer_->setPropertyValue("Playing",false);
	propertyContainer_->setPropertyValue("Trigger",!(propertyContainer_->getPropertyValue("Trigger").toBool()));
}

bool AudioElement::isPlaying()
{
	return propertyContainer_->getPropertyValue("Playing").toBool();
}

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
	updateFileObject(propertyContainer_->getPropertyValue("AudioFile").toString());
}

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

QSharedPointer<PreloadedSound> AudioElement::getSound()
{
	QString targetName = propertyContainer_->getPropertyValue("AudioFile").toString();
	Q_ASSERT(cachedSounds_.contains(targetName));
	return cachedSounds_[targetName];
}

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

void AudioElement::audioFileChanged(Property*,QVariant)
{
	updateFileObject(propertyContainer_->getPropertyValue("AudioFile").toString());
}

void AudioElement::commandChanged(Property*,QVariant)
{
	doSoundCommands();
}

void AudioElement::playOccured(QString)
{
	propertyContainer_->setPropertyValue("Playing",true);
}

void AudioElement::stopOccured(QString)
{
	propertyContainer_->setPropertyValue("Playing",false);
}

}; //namespace Picto
