#include "PictureGraphic.h"

#include <QPainter>

namespace Picto {

const QString PictureGraphic::type = "Picture Graphic";

PictureGraphic::PictureGraphic(QPoint position, QString imageFile)
: VisualElement(position)
{
	AddDefinableProperty("ImageFile",imageFile);
	if(imageFile != "" || (position != QPoint()))
	{
		initializePropertiesToDefaults();
		//We set the properties as edited because we want this to serialize out and not be mistaken for a default value.
		propertyContainer_->getProperty("Position")->setEdited();
		propertyContainer_->getProperty("ImageFile")->setEdited();
	}

	//propertyContainer_->setContainerName(type);

	//propertyContainer_->setPropertyValue("Position",position);

	//propertyContainer_->addProperty(QVariant::String,"ImageFile",imageFile);

	//draw();

	//connect(propertyContainer_.data(),
	//	    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
	//	    this,
	//		SLOT(slotPropertyValueChanged(QString, int, QVariant))
	//		);
}

void PictureGraphic::draw()
{
	QString imageFile = propertyContainer_->getPropertyValue("ImageFile").toString();
	image_.load(imageFile);

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* PictureGraphic::NewVisualElement()
{
	return new PictureGraphic;
}

//void PictureGraphic::slotPropertyValueChanged(QString propertyName, int,
//											   QVariant) //propertyValue
//{
//	if(propertyName != "Position" && propertyName != "Name")
//	{
//		draw();
//	}
//}

void PictureGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
}

bool PictureGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
