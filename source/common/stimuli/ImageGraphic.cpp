#include <QPainter>
#include "ImageGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

const QString ImageGraphic::type = "ImageGraphic";

ImageGraphic::ImageGraphic()
: VisualElement()
{
	AddDefinableProperty(QVariant::Size,"Size",QSize());
	AddDefinableProperty("ImageFile","");
}

QRect ImageGraphic::getDimensions()
{
	return QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
}

void ImageGraphic::setDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("Size",dimensions.size());
}

QString ImageGraphic::getFile()
{
	return propertyContainer_->getPropertyValue("ImageFile").toString();
}

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

VisualElement* ImageGraphic::NewVisualElement()
{
	return new ImageGraphic;
}

QSharedPointer<Asset> ImageGraphic::Create()
{
	return QSharedPointer<Asset>(new ImageGraphic());
}

void ImageGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
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
