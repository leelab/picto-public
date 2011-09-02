#include "GridGraphic.h"

#include <QPainter>

namespace Picto {

const QString GridGraphic::type = "Grid Graphic";

GridGraphic::GridGraphic()
: VisualElement()
{
	AddDefinableProperty(QVariant::Int,"Rows",5);
	AddDefinableProperty(QVariant::Int,"Columns",5);
	AddDefinableProperty(QVariant::Rect,"Dimensions",QRect());
}

void GridGraphic::draw()
{
	QRect dims = propertyContainer_->getPropertyValue("Dimensions").toRect();
	int rows = propertyContainer_->getPropertyValue("Rows").toInt();
	int cols = propertyContainer_->getPropertyValue("Columns").toInt();

	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(dims.width(),dims.height(),QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);

	//Draw it
	int rowHeight = dims.height()/rows;
	int colWidth = dims.width()/cols;
	for(int i = 1; i < rows; i++)
	{
		p.drawLine(QPoint(0,rowHeight*i),QPoint(dims.width(),rowHeight*i));
	}
	for(int i = 1; i < cols; i++)
	{
		p.drawLine(QPoint(colWidth*i,0),QPoint(colWidth*i,dims.height()));
	}
	p.end();
	image_ = image;

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* GridGraphic::NewVisualElement()
{
	return new GridGraphic;
}

void GridGraphic::postSerialize()
{
	VisualElement::postSerialize();
	setPropertyRuntimeEditable("Rows");
	setPropertyRuntimeEditable("Columns");
	draw();
}

bool GridGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;

	int rows = propertyContainer_->getPropertyValue("Rows").toInt();
	int cols = propertyContainer_->getPropertyValue("Columns").toInt();
	if(rows <= 0)
	{
		addError("GridGraphic","Rows value must be greater than zero.",xmlStreamReader);
		return false;
	}

	if(cols <= 0)
	{
		addError("GridGraphic","Columns value must be greater than zero.",xmlStreamReader);
		return false;
	}
	return true;
}

}; //namespace Picto
