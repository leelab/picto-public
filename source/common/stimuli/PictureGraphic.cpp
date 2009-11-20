#include "PictureGraphic.h"

#include <QPainter>

namespace Picto {

PictureGraphic::PictureGraphic(QPoint position, QString imageFile)
{
	parameterContainer_.setContainerName("Picture Graphic");

	parameterContainer_.addParameter(Parameter(QVariant::Point,"Position",position));

	parameterContainer_.addParameter(Parameter(QVariant::String,"ImageFile",imageFile));

	parameterContainer_.addParameter(Parameter(QVariant::String,"Name",""));

	draw();

	connect(&parameterContainer_,
		    SIGNAL(signalParameterValueChanged(QString, QVariant)),
		    this,
			SLOT(slotParameterValueChanged(QString, QVariant))
			);
}

void PictureGraphic::draw()
{
	QString imageFile = parameterContainer_.getParameterValue("ImageFile").toString();
	image_.load(imageFile);

	updateCompositingSurfaces();

	shouldRedrawImage_ = false;
}

void PictureGraphic::slotParameterValueChanged(QString parameterName,
											   QVariant) //parameterValue
{
	if(parameterName != "Position" && parameterName != "Name")
	{
		draw();
	}
}

}; //namespace Picto
