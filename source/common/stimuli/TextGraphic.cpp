#include "TextGraphic.h"

#include <QPainter>

namespace Picto {

const QString TextGraphic::type = "Text Graphic";

TextGraphic::TextGraphic(QPoint position, QRect dimensions, QColor color, QString text)
{
	propertyContainer_->setContainerName(type);

	propertyContainer_->setPropertyValue("Position",position);

	propertyContainer_->addProperty(QVariant::Rect,"Dimensions",dimensions);

	propertyContainer_->setPropertyValue("Color",color);

	propertyContainer_->addProperty(QVariant::String,"Text",text);

	draw();

	connect(propertyContainer_.data(),
		    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
		    this,
			SLOT(slotPropertyValueChanged(QString, int, QVariant))
			);
}
QRect TextGraphic::getDimensions()
{
	return propertyContainer_->getPropertyValue("Dimensions").toRect();
}

void TextGraphic::setDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("Dimensions",dimensions);
}

void TextGraphic::setHeight(int height)
{
	QRect origDims = getDimensions();
	origDims.setHeight(height);
	setDimensions(origDims);
}

void TextGraphic::setWidth(int width)
{
	QRect origDims = getDimensions();
	origDims.setWidth(width);
	setDimensions(origDims);
}

QString TextGraphic::getText()
{
	return propertyContainer_->getPropertyValue("Text").toString();
}

void TextGraphic::setText(QString text)
{
	propertyContainer_->setPropertyValue("Text",text);
}

void TextGraphic::draw()
{
	QRect dimensions = propertyContainer_->getPropertyValue("Dimensions").toRect();
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	p.setFont(QFont("Times",12));
	p.drawText(dimensions, Qt::AlignLeft | Qt::AlignVCenter,propertyContainer_->getPropertyValue("Text").toString());
	p.end();

	image_ = image;

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* TextGraphic::NewVisualElement()
{
	return new TextGraphic;
}

void TextGraphic::slotPropertyValueChanged(QString propertyName, int index,
											  QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}

}; //namespace Picto
