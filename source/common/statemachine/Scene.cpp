#include "Scene.h"

namespace Picto {

Scene::Scene()
: canvas_(QSharedPointer<Canvas>(new Canvas))
{
}

void Scene::render()
{
	//! \TODO "render" the audio stuff
	canvas_->draw();
}

void Scene::bindVisualElementsToScript(QScriptEngine &qsEngine)
{
	canvas_->bindVisualElementsToScript(qsEngine);
}

/*	\brief Converts this scene into an XML fragment
 *
 *	The Scene will look something like this when converted to XML:
 *	<Scene>
 *		<Canvas>
 *			...
 *		</Canvas>
 *		<AudioElement>
 *			...
 *		<AudioElement>
 *	</Scene>
 */
bool Scene::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Scene");
	canvas_->serializeAsXml(xmlStreamWriter);

	//! \TODO We'll need to add AudioElement stuff here...
	xmlStreamWriter->writeTextElement("AudioElement","");

	xmlStreamWriter->writeEndElement(); //Scene
	
	return true;
}
/*	\brief Converts an XML fragment into XML
 *
 *	The XML fragment will look something like this:
 *	<Scene>
 *		<Canvas>
 *			...
 *		</Canvas>
 *		<AudioElement>
 *			...
 *		<AudioElement>
 *	</Scene>
 */
bool Scene::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "Scene")
	{
		addError("Scene","Incorrect tag, expected <Scene>",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Scene") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Canvas")
		{
			canvas_ = QSharedPointer<Canvas>(new Canvas);
			if(!canvas_->deserializeFromXml(xmlStreamReader))
			{
				addError("Scene","Failed to deserialize Canvas",xmlStreamReader);
				return false;
			}
		}
		else if(name == "AudioElement")
		{
			//! \TODO deal with AudioElements here
		}
		else
		{
			addError("Scene", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("Scene", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	if(xmlStreamReader->atEnd())
	{
		addError("Scene", "Unexpected end of document", xmlStreamReader);
		return false;
	}
	return true;
}


}; //namespace Picto
