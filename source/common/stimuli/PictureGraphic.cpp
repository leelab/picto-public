#include "PictureGraphic.h"

#include <QPainter>

namespace Picto {

PictureGraphic::PictureGraphic(QPoint position, QString imageFile)
{
	propertyContainer_.setContainerName("Picture Graphic");

	propertyContainer_.addProperty(Property(QVariant::Point,"Position",position));

	propertyContainer_.addProperty(Property(QVariant::String,"ImageFile",imageFile));

	propertyContainer_.addProperty(Property(QVariant::String,"Name",""));

	draw();

	connect(&propertyContainer_,
		    SIGNAL(signalPropertyValueChanged(QString, QVariant)),
		    this,
			SLOT(slotPropertyValueChanged(QString, QVariant))
			);
}

void PictureGraphic::draw()
{
	QString imageFile = propertyContainer_.getPropertyValue("ImageFile").toString();
	image_.load(imageFile);

	updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = false;
}

void PictureGraphic::slotPropertyValueChanged(QString propertyName,
											   QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}

}; //namespace Picto
