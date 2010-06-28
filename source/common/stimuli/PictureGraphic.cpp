#include "PictureGraphic.h"

#include <QPainter>

namespace Picto {

const QString PictureGraphic::type = "Picture Graphic";

PictureGraphic::PictureGraphic(QPoint position, QString imageFile)
{
	propertyContainer_.setContainerName(type);

	propertyContainer_.setPropertyValue("Position",position);

	propertyContainer_.addProperty(Property(QVariant::String,"ImageFile",imageFile));

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

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* PictureGraphic::NewVisualElement()
{
	return new PictureGraphic;
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
