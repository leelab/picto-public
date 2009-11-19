#include "PictureGraphic.h"

#include <QPainter>

namespace Picto {

PictureGraphic::PictureGraphic(QPoint position, QString imageFile) :
	parameterContainer_(QString("Picture Graphic")),
	imageFile_(imageFile)
{
	parameterContainer_.addParameter(Parameter(QVariant::Point,"Position",position));

	parameterContainer_.addParameter(Parameter(QVariant::String,"Name",""));

	draw();
}

void PictureGraphic::draw()
{
	image_.load(imageFile_);

	updateCompositingSurfaces();

	shouldRedrawImage_ = false;
}

}; //namespace Picto
