#include "CircleGraphic.h"

#include <QPainter>

namespace Picto {

CircleGraphic::CircleGraphic(QPoint position, int radius, QColor color) :
	parameterContainer_(QString("Circle Graphic"))
{
	parameterContainer_.addParameter(Parameter(QVariant::Point,"Position",position));

	Parameter radiusParameter(QVariant::Int,"Radius",radius);
	radiusParameter.addAttribute("minimum", 1);
	radiusParameter.addAttribute("maximum", 1000);
	radiusParameter.addAttribute("singleStep", 1);
	parameterContainer_.addParameter(radiusParameter);

	parameterContainer_.addParameter(Parameter(QVariant::Color,"Color",color));

	parameterContainer_.addParameter(Parameter(QVariant::String,"Name",""));

	draw();

	connect(&parameterContainer_,
		    SIGNAL(signalParameterValueChanged(QString, QVariant)),
		    this,
			SLOT(slotParameterValueChanged(QString, QVariant))
			);
}

void CircleGraphic::draw()
{
	int radius = parameterContainer_.getParameterValue("Radius").toInt();
	QColor color = parameterContainer_.getParameterValue("Color").value<QColor>();

	QImage image(radius*2+1,radius*2+1,QImage::Format_RGB32);
	image.fill(0);
	QPainter p;
	p.begin(&image);
	p.setBrush(color);
	p.drawEllipse(image.rect());
	p.end();
	image_ = image;

	updateCompositingSurfaces();

	shouldRedrawImage_ = false;
}

void CircleGraphic::slotParameterValueChanged(QString parameterName,
											  QVariant) //parameterValue
{
	if(parameterName != "Position")
	{
		draw();
	}
}

}; //namespace Picto
