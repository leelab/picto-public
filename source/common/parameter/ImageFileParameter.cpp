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
	if(!image_.loadFromData(getFileData()))
	{
		addError("ImageFileParameter could not load the image file.");
		return false;
	}
	return true;
}

void ImageFileParameter::fileDataUpdated(const QByteArray& data)
{
	image_.loadFromData(data);
}

}; //namespace Picto
