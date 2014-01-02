#include <QPainter>
#include "ImageGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief I believe that this is no longer used.  It sbould probably be deleted.*/
const QString ImageGraphic::type = "ImageGraphic";

/*! \brief Constructs an ImageGraphic.
 *	\details Adds an ImageFile Property for handdling the name of the ImageFile element that will be displayed.  Adds a Size Property
 *	to define the size of the image in the Scene.
 */
ImageGraphic::ImageGraphic()
: VisualElement()
{
	AddDefinableProperty(QVariant::Size,"Size",QSize());
	AddDefinableProperty("ImageFile","");
}

/*! \brief Returns a QRect with the dimensions (width, height) of the image.
 *	\note The returned value contains no position information.  Only the Width, Height
 *	values should be used.
*/
QRect ImageGraphic::getDimensions()
{
	return QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
}

/*! \brief Set the dimensions (width, height) of the image.
 *	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
*/
void ImageGraphic::setDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("Size",dimensions.size());
}

/*! \brief Returns the name of the ImageFile element that contains the Image file used by this ImageGraphic.*/
QString ImageGraphic::getFile()
{
	return propertyContainer_->getPropertyValue("ImageFile").toString();
}

/*! \brief Sets the name of the ImageFile element that contains the Image file to be used by this ImageGraphic.*/
void ImageGraphic::setFile(QString fileObjectName)
{
	propertyContainer_->setPropertyValue("ImageFile",fileObjectName);
}

QPoint ImageGraphic::getPositionOffset()
{
	return posOffset_;
}

void ImageGraphic::draw()
{
	QString fileObjName = propertyContainer_->getPropertyValue("ImageFile").toString();
	if(fileObj_.isNull() || (fileObj_.toStrongRef()->getName() != fileObjName))
		updateFileObject();
	if(fileObj_.isNull())
		return;
	QRect dimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	image_ = QImage::fromData(fileObj_.toStrongRef()->getFileData()).scaled(dimensions.width(),dimensions.height());
	posOffset_ = QPoint(image_.width()/2.0,image_.height()/2.0);

	shouldUpdateCompositingSurfaces_ = true;
}

/*! \brief This is no longer used by parts of Picto that are being used.  It sbould probably be deleted.*/
VisualElement* ImageGraphic::NewVisualElement()
{
	return new ImageGraphic;
}

/*! \brief Creates a new ImageGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> ImageGraphic::Create()
{
	return QSharedPointer<Asset>(new ImageGraphic());
}

void ImageGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
	propertyContainer_->getProperty("Color")->setVisible(false);
}

bool ImageGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

void ImageGraphic::scriptableContainerWasReinitialized()
{
	VisualElement::scriptableContainerWasReinitialized();
	updateFileObject();
}

/*! \brief Extends VisualElement::executeSearchAlgorithm() to look for the input searchRequest in the ImageFile Parameter.*/
bool ImageGraphic::executeSearchAlgorithm(SearchRequest searchRequest)
{
	if(VisualElement::executeSearchAlgorithm(searchRequest))
		return true;
	switch(searchRequest.type)
	{
	case SearchRequest::STRING:
		{
			//Search my control target for the string
			QString fileName = propertyContainer_->getPropertyValue("ImageFile").toString();
			if(!fileName.isNull() && fileName.contains(searchRequest.query,searchRequest.caseSensitive?Qt::CaseSensitive:Qt::CaseInsensitive))
				return true;
		}
		break;
	};
	return false;
}

/*! \brief Finds the ImageFileParameter with the name set in the ImageFile Poperty and redraws
 *	the underlying image based on that file.
*/
void ImageGraphic::updateFileObject()
{
	QList<QWeakPointer<Scriptable>> scriptables = getScriptableList();
	QString targetName = propertyContainer_->getPropertyValue("ImageFile").toString();
	foreach(QWeakPointer<Scriptable> scriptable,scriptables)
	{
		if(scriptable.isNull())
			continue;
		if(scriptable.toStrongRef()->getName() == targetName && scriptable.toStrongRef()->inherits("Picto::ImageFileParameter"))
		{
			fileObj_ = scriptable.toStrongRef().staticCast<FileParameter>();
			draw();
			break;
		}
	}
}
}; //namespace Picto
