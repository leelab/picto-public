#include <QPainter>

#include "TextGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

	const QString TextGraphic::type = "Text Graphic";

/*! \brief Creates a new TextGraphic object at the input position and with the input dimensions, color and text string.
 *	\details Adds the following Properties:
 *	- Size: Stores the size of the graphic (width, height).
 *	- Text: Stores the text that will be displayed in the graphic.
 */
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
}

void TextGraphic::draw()
{
	QRect dimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());

	//Don't waste our time (or fill up our debug output) if nothing will be drawn
	if (dimensions.width() == 0 || dimensions.height() == 0 || !dimensions.isValid())
	{
		return;
	}

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

/*! \brief Creates a new TextGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> TextGraphic::Create()
{
	return QSharedPointer<Asset>(new TextGraphic());
}

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
