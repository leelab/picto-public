#include "Layer.h"

#include "../stimuli/VisualElementFactory.h"
#include "../compositor/RenderingTarget.h"
#include "../compositor/VisualTarget.h"
#include "../engine/PictoEngine.h"

namespace Picto {

Layer::Layer()
	: order_(0),
	 frame_(0)
{
	name_ = "";
	elapsedTime_.start();
}

void Layer::addVisualElement(QSharedPointer<VisualElement> v)
{
	//add the appropriate compositing surfaces to the element
	QSharedPointer<Picto::CompositingSurface> compositingSurface;

	QList<QSharedPointer<RenderingTarget> > renderingTargets;
	renderingTargets = Engine::PictoEngine::getRenderingTargets();

	foreach(QSharedPointer<RenderingTarget> renderTarget, renderingTargets)
	{
		compositingSurface = renderTarget->generateCompositingSurface();
		v->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);
	}

	visualElements_.push_back(v);
}

/*! \brief This is where the actual rendering occurs
 *
 *	This gets called by the Canvas for every rendering taget 
 *  currently being used by the engine.
 */
void Layer::draw(QSharedPointer<VisualTarget> visualTarget)
{
	//run through all our visualElements updating the animation and drawing
	foreach(QSharedPointer<VisualElement> visualElement, visualElements_)
	{
		visualElement->updateAnimation(frame_,elapsedTime_);
		visualTarget->draw(visualElement->getPosition(),visualElement->getCompositingSurface(visualTarget->getTypeName()));
	}

	frame_++;
	elapsedTime_.restart();
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
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "Layer")
	{
		addError("Layer","Incorrect tag, expected <Layer>",xmlStreamReader);
		return false;
	}

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
			{
				addError("Layer", "Failed to deserialize a VisualElement", xmlStreamReader);
				return false;
			}
			addVisualElement(newVisualElement);
		}
		else
		{
			addError("Layer", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("Layer", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	return true;
}

}; //namespace Picto
