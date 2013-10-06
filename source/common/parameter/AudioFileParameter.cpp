#include <QFile>
#include "AudioFileParameter.h"
#include "../memleakdetect.h"

namespace Picto {

AudioFileParameter::AudioFileParameter()
: FileParameter()
{
}

QSharedPointer<Asset> AudioFileParameter::Create()
{
	return QSharedPointer<Asset>(new AudioFileParameter());
}

void AudioFileParameter::postDeserialize()
{
	FileParameter::postDeserialize();
}

bool AudioFileParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!FileParameter::validateObject(xmlStreamReader))
		return false;
	//if(!image_.loadFromData(getFileData()))
	//{
	//	addError("AudioFileParameter could not load the image file.");
	//	return false;
	//}
	return true;
}

void AudioFileParameter::fileDataUpdated(const QByteArray& data)
{
	//image_.loadFromData(data);
}

}; //namespace Picto
