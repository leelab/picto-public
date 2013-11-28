#include <QFile>
#include "ImageFileParameter.h"
#include "../memleakdetect.h"

namespace Picto {

ImageFileParameter::ImageFileParameter()
: FileParameter()
{
}
/*! \brief Creates a new ImageFileParameter object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> ImageFileParameter::Create()
{
	return QSharedPointer<Asset>(new ImageFileParameter());
}

void ImageFileParameter::postDeserialize()
{
	FileParameter::postDeserialize();
}
/*! \brief Extends FileParameter::validateObject() to verify that this AudioFileParameter's file is 
 *	not empty and of a supported file type.
 */
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

/*! \brief Implements FileParameter::fileDataUpdated() to load the stored QImage from 
 *	file binary data whenever that file binary data changes.
 *	\details This appears to be inefficiently implemented.  No one has access to image_ and
 *	the only reason to load it from binary data is to verify that the binary data is in a
 *	supported format.  Ideally then we should do the load here and then just store a boolean
 *	indicating if the load worked or not, which we can then use in the validateObject() function.
 *	This is not what we are doing though.  In the validateObject() function we are just performing
 *	the load test again.  This should be looked into.
*/
void ImageFileParameter::fileDataUpdated(const QByteArray& data)
{
	image_.loadFromData(data);
}

}; //namespace Picto
