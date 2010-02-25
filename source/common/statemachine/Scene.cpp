#include "Scene.h"

namespace Picto {

Scene::Scene()
{
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
	if(!xmlStreamReader->isStartElement())
		return false;
	if(xmlStreamReader->name() != "Scene")
		return false;

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
				return false;
		}
		else if(name == "AudioElement")
		{
			//! \TODO deal with AudioElements here
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
