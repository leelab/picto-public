#include "Canvas.h"
#include "../engine/PictoEngine.h"
#include "../compositor/RenderingTarget.h"
#include "../compositor/VisualTarget.h"


namespace Picto {

bool layerLessThan(const QSharedPointer<Layer> &l1, const QSharedPointer<Layer> &l2) 
{ 
	return l1->getOrder() < l2->getOrder(); 
}

Canvas::Canvas()  :
	backgroundColor_(QColor(Qt::black))
{
	DefinePlaceholderTag("Layers");
	AddDefinableProperty(QVariant::Color,"BackgroundColor","");
	AddDefinableObjectFactory( "Layer",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Layer::Create))) );

}

void Canvas::bindToScriptEngine(QSharedPointer<QScriptEngine> qsEngine)
{
	foreach(QSharedPointer<Layer> layer, layers_)
	{
		layer->bindToScriptEngine(qsEngine);
	}

}

//Resets the canvas
void Canvas::reset()
{
	foreach(QSharedPointer<Layer> layer, layers_)
	{
		layer->reset();
	}
}

void Canvas::draw(QSharedPointer<Engine::PictoEngine> engine)
{
	//! \todo deal with the background layer color

	//Grab the RenderingTargets from the engine
	QList<QSharedPointer< RenderingTarget> > renderingTargets;
	renderingTargets = engine->getRenderingTargets();

	foreach(QSharedPointer<RenderingTarget> renderTarget, renderingTargets)
	{
		QSharedPointer<VisualTarget> visualTarget = renderTarget->getVisualTarget();

		//The Layers are sorted as they get added, so we can simply run through them in order
		foreach(QSharedPointer<Layer> layer, layers_)
		{
			layer->draw(visualTarget, engine);
		}

		visualTarget->present();
	}
}

void Canvas::addLayer(QSharedPointer<Layer> layer)
{ 
	layers_.push_back(layer); 
	qSort(layers_.begin(), layers_.end(), &layerLessThan);

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
//bool Canvas::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("Canvas");
//
//	xmlStreamWriter->writeStartElement("Layers");
//
//	xmlStreamWriter->writeStartElement("BackgroundLayer");
//	xmlStreamWriter->writeAttribute("color",backgroundColor_.name());
//	xmlStreamWriter->writeEndElement();
//
//	foreach(QSharedPointer<Layer> layer, layers_)
//		layer->toXml(xmlStreamWriter);
//
//	xmlStreamWriter->writeEndElement(); //Layers
//	xmlStreamWriter->writeEndElement(); //Canvas
//	
//	return true;
//
//	return false;
//}
//
///*!	\brief Creates a Canvas from an XML fragment
// *
// *	The XML fragment for a canvas will look something like this:
// *	<Canvas>
// *		<Layers>
// *			<BackgroundLayer color="#FF00FF"/>
// *			<Layer>
// *				...
// *			</Layer>
// *			<Layer>
// *				...
// *			</Layer>
// *		</Layers>
// *	</Canvas>
// */
//bool Canvas::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//start off by clearing out all existing layers
//	layers_.clear();
//
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "Canvas")
//	{
//		addError("Canvas","Incorrect tag, expected <Canvas>",xmlStreamReader);
//		return false;
//	}
//
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name() == "Canvas") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->name().toString();
//		if(name == "Layers")
//		{
//			//we don't need to do anything here
//		}
//		else if(name == "BackgroundLayer")
//		{
//			QString colorName = xmlStreamReader->attributes().value("color").toString();
//			backgroundColor_.setNamedColor(colorName);
//		}
//		else if(name == "Layer")
//		{
//			QSharedPointer<Layer> newLayer(new Layer);
//			if(!newLayer->fromXml(xmlStreamReader))
//				return false;
//			layers_.push_back(newLayer);
//		}
//		else
//		{
//			addError("Canvas", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//		xmlStreamReader->readNext();
//	}
//
//	if(xmlStreamReader->atEnd())
//	{
//		addError("Canvas", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//	return true;
//}

bool Canvas::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	backgroundColor_.setNamedColor(propertyContainer_->getPropertyValue("BackgroundColor").toString());
	
	QList<QSharedPointer<Asset>> newLayers = getGeneratedChildren("Layer");
	foreach(QSharedPointer<Asset> newLayer,newLayers)
	{
		layers_.push_back(newLayer.staticCast<Layer>());
	}
	
	return true;
}

}; //namespace Picto
