#include "PictureGraphic.h"

#include <QPainter>

namespace Picto {

const QString PictureGraphic::type = "Picture Graphic";

PictureGraphic::PictureGraphic(QPoint position, QString imageFile)
{
	propertyContainer_->setContainerName(type);

	propertyContainer_->setPropertyValue("Position",position);

	propertyContainer_->addProperty(QVariant::String,"ImageFile",imageFile);

	draw();

	connect(propertyContainer_.data(),
		    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
		    this,
			SLOT(slotPropertyValueChanged(QString, int, QVariant))
			);
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

void PictureGraphic::slotPropertyValueChanged(QString propertyName, int index,
											   QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}

}; //namespace Picto
