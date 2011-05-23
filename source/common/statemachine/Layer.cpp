#include "Layer.h"

#include "../stimuli/VisualElementFactory.h"
#include "../compositor/RenderingTarget.h"
#include "../compositor/VisualTarget.h"
#include "../engine/PictoEngine.h"
#include "../stimuli/ArrowGraphic.h"
#include "../stimuli/BoxGraphic.h"
#include "../stimuli/CircleGraphic.h"
#include "../stimuli/EllipseGraphic.h"
#include "../stimuli/LineGraphic.h"
#include "../stimuli/PictureGraphic.h"
#include "../stimuli/RandomlyFilledGridGraphic.h"
#include "../stimuli/TextGraphic.h"

namespace Picto {

Layer::Layer()
	: order_(0),
	 frame_(0)
{
	AddDefinableProperty("Name","");
	AddDefinableProperty(QVariant::Int,"Order",0);

	QSharedPointer<SerializableFactory> factory(new SerializableFactory());
	factory->addSerializableType(ArrowGraphic::type,
		QSharedPointer<SerializableFactory>(new SerializableFactory(0,-1,SerializableFactory::NewSerializableFnPtr(ArrowGraphic::Create))));
	factory->addSerializableType(BoxGraphic::type,
		QSharedPointer<SerializableFactory>(new SerializableFactory(0,-1,SerializableFactory::NewSerializableFnPtr(BoxGraphic::Create))));
	factory->addSerializableType(CircleGraphic::type,
		QSharedPointer<SerializableFactory>(new SerializableFactory(0,-1,SerializableFactory::NewSerializableFnPtr(CircleGraphic::Create))));
	factory->addSerializableType(EllipseGraphic::type,
		QSharedPointer<SerializableFactory>(new SerializableFactory(0,-1,SerializableFactory::NewSerializableFnPtr(EllipseGraphic::Create))));
	factory->addSerializableType(LineGraphic::type,
		QSharedPointer<SerializableFactory>(new SerializableFactory(0,-1,SerializableFactory::NewSerializableFnPtr(LineGraphic::Create))));
	factory->addSerializableType(PictureGraphic::type,
		QSharedPointer<SerializableFactory>(new SerializableFactory(0,-1,SerializableFactory::NewSerializableFnPtr(PictureGraphic::Create))));
	factory->addSerializableType(RandomlyFilledGridGraphic::type,
		QSharedPointer<SerializableFactory>(new SerializableFactory(0,-1,SerializableFactory::NewSerializableFnPtr(RandomlyFilledGridGraphic::Create))));
	factory->addSerializableType(TextGraphic::type,
		QSharedPointer<SerializableFactory>(new SerializableFactory(0,-1,SerializableFactory::NewSerializableFnPtr(TextGraphic::Create))));
	AddDefinableObjectFactory("VisualElement",factory);


	name_ = "";
	elapsedTime_.start();
}

void Layer::bindToScriptEngine(QSharedPointer<QScriptEngine> qsEngine)
{
	foreach(QSharedPointer<VisualElement> visualElement, visualElements_)
	{
		visualElement->bindToScriptEngine(qsEngine);
	}

	foreach(QSharedPointer<VisualElement> visualElement, unaddedVisualElements_)
	{
		visualElement->bindToScriptEngine(qsEngine);
	}

}

//!  Resets the layer
void Layer::reset()
{
	foreach(QSharedPointer<VisualElement> visualElement, visualElements_)
	{
		visualElement->reset();
	}

	foreach(QSharedPointer<VisualElement> visualElement, unaddedVisualElements_)
	{
		visualElement->reset();
	}
}

void Layer::addVisualElement(QSharedPointer<VisualElement> v)
{
	unaddedVisualElements_.push_back(v);
}

/*! \brief This is where the actual rendering occurs
 *
 *	This gets called by the Canvas for every rendering taget 
 *  currently being used by the engine.
 */
void Layer::draw(QSharedPointer<VisualTarget> visualTarget, QSharedPointer<Engine::PictoEngine> engine)
{
	//See if we need to change the rendering targets
	if(!unaddedVisualElements_.isEmpty())
	{
		QList<QSharedPointer<RenderingTarget> > renderingTargets;
		renderingTargets = engine->getRenderingTargets();
		QSharedPointer<Picto::CompositingSurface> compositingSurface;

		//add the appropriate compositing surfaces to the element
		//foreach(QSharedPointer<VisualElement> element, unaddedVisualElements_)
		for(int i=0; i<unaddedVisualElements_.length(); i++)
		{
			foreach(QSharedPointer<RenderingTarget> renderTarget, renderingTargets)
			{
				compositingSurface = renderTarget->generateCompositingSurface();
				unaddedVisualElements_[i]->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);
			}
			visualElements_.append(unaddedVisualElements_[i]);
		}

		unaddedVisualElements_.clear();
	}
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
//bool Layer::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("Layer");
//
//	xmlStreamWriter->writeTextElement("Name",name_);
//	xmlStreamWriter->writeTextElement("Order", QString("%1").arg(order_));
//
//	foreach(QSharedPointer<VisualElement> v, visualElements_)
//		v->toXml(xmlStreamWriter);
//	xmlStreamWriter->writeEndElement(); //Layer
//	return true;
//}
//
///*!	\brief Converts an XML fragment to a canvas
// *
// *	The XML fragment for a layer will look something like this:
// *	<Layer>
// *		<Name> Default</Name>
// *		<Order>1</Order>
// *		<VisualElement type="Circle Graphic">
// *			...
// *		</VisualElement>
// *		<VisualElement type="Arrow Graphic">
// *			...
// *		</VisualElement>
// *	</Layer>
// *
// */
//bool Layer::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//start off by clearing out all existing layers
//	visualElements_.clear();
//
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "Layer")
//	{
//		addError("Layer","Incorrect tag, expected <Layer>",xmlStreamReader);
//		return false;
//	}
//
//	//We'll need a visual element factory
//	VisualElementFactory visualElementFactory;
//
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Layer") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->name().toString();
//		if(name == "Name")
//		{
//			name_ = xmlStreamReader->readElementText();
//		}
//		else if(name=="Order")
//		{
//			order_ = xmlStreamReader->readElementText().toInt();
//		}
//		else if(name == "VisualElement")
//		{
//			QString visualElementType = xmlStreamReader->attributes().value("type").toString();
//			QSharedPointer<VisualElement> newVisualElement = visualElementFactory.generateVisualElement(visualElementType);
//			if(newVisualElement.isNull())
//			{
//				addError("Layer", "Unrecognized visual element type: "+visualElementType,xmlStreamReader);
//				return false;
//			}
//			if(!newVisualElement->fromXml(xmlStreamReader))
//			{
//				addError("Layer", "Failed to deserialize a VisualElement", xmlStreamReader);
//				return false;
//			}
//			addVisualElement(newVisualElement);
//		}
//		else
//		{
//			addError("Layer", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//		xmlStreamReader->readNext();
//	}
//
//	if(xmlStreamReader->atEnd())
//	{
//		addError("Layer", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//
//	return true;
//}

bool Layer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//start off by clearing out all existing layers
	visualElements_.clear();

	name_ = propertyContainer_->getPropertyValue("Name").toString();
	order_ = propertyContainer_->getPropertyValue("Order").toInt();

	QList<QSharedPointer<Serializable>> newVisElems = getGeneratedChildren("VisualElement");
	foreach(QSharedPointer<Serializable> newVisElem,newVisElems)
	{
		addVisualElement(newVisElem.staticCast<VisualElement>());
	}
	return true;
}

}; //namespace Picto
