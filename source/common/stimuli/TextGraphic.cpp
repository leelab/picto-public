#include <QPainter>

#include "TextGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

const QString TextGraphic::type = "Text Graphic";

TextGraphic::TextGraphic(QPoint position, QRect dimensions, QColor color, QString text)
: VisualElement(position,color)
{
	AddDefinableProperty(QVariant::Size,"Size",dimensions.size());
	AddDefinableProperty(QVariant::String,"Text",text);
	if((position != QPoint()) || (dimensions != QRect()) || (color != QColor()) || (text != QString()))
	{
		initializePropertiesToDefaults();
		//We set the properties as edited because we want this to serialize out and not be mistaken for a default value.
		propertyContainer_->getProperty("Position")->setEdited();
		propertyContainer_->getProperty("Color")->setEdited();
		propertyContainer_->getProperty("Size")->setEdited();
		propertyContainer_->getProperty("Text")->setEdited();
	}

	//propertyContainer_->setContainerName(type);

	//propertyContainer_->setPropertyValue("Position",position);

	//propertyContainer_->addProperty(QVariant::Size,"Size",dimensions.size());

	//propertyContainer_->setPropertyValue("Color",color);

	//propertyContainer_->addProperty(QVariant::String,"Text",text);

	//draw();

	//connect(propertyContainer_.data(),
	//	    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
	//	    this,
	//		SLOT(slotPropertyValueChanged(QString, int, QVariant))
	//		);
}

void TextGraphic::draw()
{
	QRect dimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
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

QSharedPointer<Asset> TextGraphic::Create()
{
	return QSharedPointer<Asset>(new TextGraphic());
}

//void TextGraphic::slotPropertyValueChanged(QString propertyName, int,
//											  QVariant) //propertyValue
//{
//	if(propertyName != "Position" && propertyName != "Name")
//	{
//		draw();
//	}
//}

void TextGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
	setPropertyRuntimeEditable("Text");
}

bool TextGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
