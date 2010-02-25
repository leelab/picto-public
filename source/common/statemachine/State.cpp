#include "State.h"

namespace Picto {

State::State()
{
	type_ = "State";

	//at some point, these should actually do something, but for the moment
	//we'll leave them as -1 to show that they aren't being used
	revision_ = -1;
	engineNeeded_ = -1;

}

QString State::run()
{
	//This is where the rendering actually happens
	while(true)
	{
		//Draw the scene
		//! \TODO implement Scene::draw()

		//check all of the control elements
		//This foreach loop was causing the compiler to crash.
		//Probably due to the fact that it was empty
		//foreach(ControlElement control, controlElements_)
		//{
			//! \TODO implement the control elements here
			
			/* PSEUDOCODE
			if(control.isDone())
				return control.result();
			*/
		//}

		//If there are no control elements, then we just return with an empty string
		//if(controlElements_.isEmpty())
			return "";
	}
}

/*!	\Brief Turns a state object into an XML framgent.
 *
 *	The XML fragment for a state will look like this:
 * 
 *	<State>
 *		<ID>{19C122D6-ECCC-43ee-8337-565A66944DE4}</ID>
 *		<Revision>1</Revision>
 *		<EngineNeeded>1</EngineNeeded>
 *		<Name>Fixation</Name>
 *		<ControlElements>
 *			...
 *		</ControlElements>
 *		<Scene>
 *			...
 *		</Scene>
 *	</State>
 */
bool State::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("State");
	
	xmlStreamWriter->writeTextElement("ID",getId().toString());
	xmlStreamWriter->writeTextElement("Revision",QString("%1").arg(revision_));
	xmlStreamWriter->writeTextElement("EngineNeeded",QString("%1").arg(engineNeeded_));
	xmlStreamWriter->writeTextElement("Name",name_);

	xmlStreamWriter->writeStartElement("ControlElements");
	/*ControlElements aren't implemented yet...
	foreach(QSharedPointer<ControlElement> c, controlElements_)
		c->serializeAsXml(xmlStreamWriter);*/
	xmlStreamWriter->writeEndElement();

	scene_->serializeAsXml(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //State
	return true;
}

/*!	\Brief Turns a an XML fragment into a State object
 *
 *	The XML fragment for a state will look like this:
 * 
 *	<State>
 *		<ID>{19C122D6-ECCC-43ee-8337-565A66944DE4}</ID>
 *		<Revision>1</Revision>
 *		<EngineNeeded>1</EngineNeeded>
 *		<Name>Fixation</Name>
 *		<ControlElements>
 *			...
 *		</ControlElements>
 *		<Scene>
 *			...
 *		</Scene>
 *	</State>
 */
bool State::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement())
		return false;
	if(xmlStreamReader->name() != "State")
		return false;

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "State") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "ID")
		{
			QString idStr = xmlStreamReader->readElementText();
			setId(QUuid(idStr));
			registerObject();
		}
		else if(name == "Revision")
		{
			bool ok;
			revision_= xmlStreamReader->readElementText().toInt(&ok);
			if(!ok)
				return false;
		}
		else if(name == "EngineNeeded")
		{
			bool ok;
			engineNeeded_= xmlStreamReader->readElementText().toInt(&ok);
			if(!ok)
				return false;
		}
		else if(name == "Name")
		{
			name_ = xmlStreamReader->readElementText();
		}
		else if(name == "ControlElements")
		{
			//do nothing
		}
		else if(name == "ControlElement")
		{
			//! \TODO fill this in...
			//we'll need to generate the correct ype of controlElement
			//(probably using a factory), and then call deserialize on it
		}
		else if(name == "Scene")
		{
			QSharedPointer<Scene> newScene(new Scene);
			if(!newScene->deserializeFromXml(xmlStreamReader))
				return false;
			scene_ = newScene;
		}

		else
		{
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
		return false;

	return true;
}


}; //namespace Picto
