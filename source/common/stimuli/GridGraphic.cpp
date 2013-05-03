#include <QPainter>
#include "GridGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

const QString GridGraphic::type = "Grid Graphic";

GridGraphic::GridGraphic()
: VisualElement()
{
	AddDefinableProperty(QVariant::Int,"Rows",5);
	AddDefinableProperty(QVariant::Int,"Columns",5);
	AddDefinableProperty(QVariant::Size,"Size",QSize());
}

void GridGraphic::draw()
{
	QRect dims = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
	int rows = propertyContainer_->getPropertyValue("Rows").toInt();
	int cols = propertyContainer_->getPropertyValue("Columns").toInt();

	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(dims.width(),dims.height(),QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	QPen pen(color);
	pen.setWidth(0);	//0 means its a cosmetic line that should not disappear at any zoom level
	p.setPen(pen);

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

QSharedPointer<Asset> GridGraphic::Create()
{
	return QSharedPointer<Asset>(new GridGraphic());
}

void GridGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
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
		addError("Rows value must be greater than zero.");
		return false;
	}

	if(cols <= 0)
	{
		addError("Columns value must be greater than zero.");
		return false;
	}
	return true;
}

}; //namespace Picto
