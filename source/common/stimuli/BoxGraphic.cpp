#include "BoxGraphic.h"

#include <QPainter>

namespace Picto {

BoxGraphic::BoxGraphic(QPoint position, QRect dimensions, QColor color)
{
	parameterContainer_.setContainerName("Box Graphic");

	parameterContainer_.addParameter(Parameter(QVariant::Point,"Position",position));

	Parameter dimensionsParameter(QVariant::Rect,"Dimensions",dimensions);
	parameterContainer_.addParameter(dimensionsParameter);

	parameterContainer_.addParameter(Parameter(QVariant::Color,"Color",color));

	parameterContainer_.addParameter(Parameter(QVariant::String,"Name",""));

	draw();

	connect(&parameterContainer_,
		    SIGNAL(signalParameterValueChanged(QString, QVariant)),
		    this,
			SLOT(slotParameterValueChanged(QString, QVariant))
			);
}

void BoxGraphic::draw()
{
	QRect dimensions = parameterContainer_.getParameterValue("Dimensions").toRect();
	QColor color = parameterContainer_.getParameterValue("Color").value<QColor>();

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	p.drawRect(dimensions);
	p.end();
	image_ = image;

	updateCompositingSurfaces();

	shouldRedrawImage_ = false;
}

void BoxGraphic::slotParameterValueChanged(QString parameterName,
											  QVariant) //parameterValue
{
	if(parameterName != "Position" && parameterName != "Name")
	{
		draw();
	}
}

}; //namespace Picto
