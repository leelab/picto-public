#include <QPainter>
#include "TextFactoryGraphic.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

	const QString TextFactoryGraphic::type = "Text Factory Graphic";

	TextFactoryGraphic::TextFactoryGraphic(QPoint position, QRect dimensions, QColor color, QString text)
:
VisualElement(position,color)
{
	AddDefinableProperty(QVariant::Size, "Size", dimensions.size());
	AddDefinableProperty(QVariant::String, "Text", text);
	if ((position != QPoint()) || (dimensions != QRect()) || (color != QColor()) || (text != QString()))
	{
		initializePropertiesToDefaults();
		//We set the properties as edited because we want this to serialize out and not be mistaken for a default value.
		propertyContainer_->getProperty("Position")->setEdited();
		propertyContainer_->getProperty("Color")->setEdited();
		propertyContainer_->getProperty("Size")->setEdited();
		propertyContainer_->getProperty("Text")->setEdited();
	}
}
void TextFactoryGraphic::draw()
{
	QRect dimensions = QRect(QPoint(), propertyContainer_->getPropertyValue("Size").toSize());

	//Don't waste our time (or fill up our debug output) if nothing will be drawn
	if (dimensions.width() == 0 || dimensions.height() == 0 || !dimensions.isValid())
	{
		return;
	}

	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(dimensions.width(), dimensions.height(), QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	p.setFont(QFont("Times", 12));
	p.drawText(dimensions, Qt::AlignLeft | Qt::AlignVCenter, propertyContainer_->getPropertyValue("Text").toString());
	p.end();

	image_ = image;

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

/*! \brief Creates a new TextFactoryGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> TextFactoryGraphic::Create()
{
	return QSharedPointer<Asset>(new TextFactoryGraphic());
}

void TextFactoryGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
	setPropertyRuntimeEditable("TextFactory");
}

bool TextFactoryGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if (!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
