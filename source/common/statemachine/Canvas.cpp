#include "Canvas.h"

namespace Picto {

Canvas::Canvas()
: backgroundColor_(QColor(Qt::black))
{
}

/*!	\brief Converts this canvas to an XML fragment
 *
 *	The XML fragment for a canvas will look something like this:
 *	<Canvas>
 *		<Layers>
 *			<BackgroundLayer color="#FF00FF"/>
 *			<Layer>
 *				...
 *			</Layer>
 *			<Layer>
 *				...
 *			</Layer>
 *		</Layers>
 *	</Canvas>
 */
bool Canvas::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Canvas");

	xmlStreamWriter->writeStartElement("Layers");

	xmlStreamWriter->writeStartElement("BackgroundLayer");
	xmlStreamWriter->writeAttribute("color",backgroundColor_.name());
	xmlStreamWriter->writeEndElement();

	foreach(QSharedPointer<Layer> layer, layers_)
		layer->serializeAsXml(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //Layers
	xmlStreamWriter->writeEndElement(); //Canvas
	
	return true;

	return false;
}

/*!	\brief Creates a Canvas from an XML fragment
 *
 *	The XML fragment for a canvas will look something like this:
 *	<Canvas>
 *		<Layers>
 *			<BackgroundLayer color="#FF00FF"/>
 *			<Layer>
 *				...
 *			</Layer>
 *			<Layer>
 *				...
 *			</Layer>
 *		</Layers>
 *	</Canvas>
 */
bool Canvas::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//start off by clearing out all existing layers
	layers_.clear();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement())
		return false;
	if(xmlStreamReader->name() != "Canvas")
		return false;

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Canvas") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Layers")
		{
			//we don't need to do anything here
		}
		else if(name == "BackgroundLayer")
		{
			QString colorName = xmlStreamReader->attributes().value("color").toString();
			backgroundColor_.setNamedColor(colorName);
		}
		else if(name == "Layer")
		{
			QSharedPointer<Layer> newLayer(new Layer);
			if(!newLayer->deserializeFromXml(xmlStreamReader))
				return false;
			layers_.push_back(newLayer);
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
