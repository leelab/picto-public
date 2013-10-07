#include <QFile>
#include "ImageFileParameter.h"
#include "../memleakdetect.h"

namespace Picto {

ImageFileParameter::ImageFileParameter()
: FileParameter()
{
}

QSharedPointer<Asset> ImageFileParameter::Create()
{
	return QSharedPointer<Asset>(new ImageFileParameter());
}

void ImageFileParameter::postDeserialize()
{
	FileParameter::postDeserialize();
}

bool ImageFileParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!FileParameter::validateObject(xmlStreamReader))
		return false;
	QByteArray fileData = getFileData();
	if(fileData.isEmpty())
	{
		addError("Image file was empty or could not be loaded.");
		return false;
	}
	if(!image_.loadFromData(getFileData()))
	{
		addError("The image file appears to have an unsupported file type.");
		return false;
	}
	return true;
}

void ImageFileParameter::fileDataUpdated(const QByteArray& data)
{
	image_.loadFromData(data);
}

}; //namespace Picto
