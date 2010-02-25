#include "Layer.h"

#include "../stimuli/VisualElementFactory.h"

namespace Picto {

Layer::Layer()
	: order_(0)
{
	name_ = "";
}

/*!	\brief Converts this canvas to an XML fragment
 *
 *	The XML fragment for a layer will look something like this:
 *	<Layer>
 *		<Name> Default</Name>
 *		<Order>1</Order>
 *		<VisualElement type="Circle Graphic">
 *			...
 *		</VisualElement>
 *		<VisualElement type="Arrow Graphic">
 *			...
 *		</VisualElement>
 *	</Layer>
 *
 */
bool Layer::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Layer");

	xmlStreamWriter->writeTextElement("Name",name_);
	xmlStreamWriter->writeTextElement("Order", QString("%1").arg(order_));

	foreach(QSharedPointer<VisualElement> v, visualElements_)
		v->serializeAsXml(xmlStreamWriter);
	xmlStreamWriter->writeEndElement(); //Layer
	return true;
}

/*!	\brief Converts an XML fragment to a canvas
 *
 *	The XML fragment for a layer will look something like this:
 *	<Layer>
 *		<Name> Default</Name>
 *		<Order>1</Order>
 *		<VisualElement type="Circle Graphic">
 *			...
 *		</VisualElement>
 *		<VisualElement type="Arrow Graphic">
 *			...
 *		</VisualElement>
 *	</Layer>
 *
 */
bool Layer::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//start off by clearing out all existing layers
	visualElements_.clear();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement())
		return false;
	if(xmlStreamReader->name() != "Layer")
		return false;

	//We'll need a visual element factory
	VisualElementFactory visualElementFactory;

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Layer") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Name")
		{
			name_ = xmlStreamReader->readElementText();
		}
		else if(name=="Order")
		{
			order_ = xmlStreamReader->readElementText().toInt();
		}
		else if(name == "VisualElement")
		{
			QString visualElementType = xmlStreamReader->attributes().value("type").toString();
			QSharedPointer<VisualElement> newVisualElement = visualElementFactory.generateVisualElement(visualElementType);
			if(!newVisualElement->deserializeFromXml(xmlStreamReader))
				return false;
			visualElements_.push_back(newVisualElement);
		}
		else
		{
			return false;
		}
		xmlStreamReader->readNext();
	}

	return true;
}

}; //namespace Picto
