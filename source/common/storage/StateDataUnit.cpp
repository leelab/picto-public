#include "StateDataUnit.h"

namespace Picto {

StateDataUnit::StateDataUnit()
{
}

void StateDataUnit::setTransition(QSharedPointer<Transition> transition)
{
	setTransition(transition->getAssetId());
}
void StateDataUnit::setTransition(int id)
{
	id_ = id;
}

/*! \brief Turns the StateDataUnit into an XML fragment
 *
 *	The XML will look like this:
 *	<StateDataUnit timestamp = 123.456 statemachinepath="state machine name">
 *		<Transition>
 *			<Source>TestState</Source>
 *			<SourceResult>Success</SourceResult>
 *			<Destination>Done</Destination>
 *		</Transition>
 *	</StateDataUnit>
 */
bool StateDataUnit::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("SDU");
	xmlStreamWriter->writeAttribute("f",QString::number(actionFrame_));
	xmlStreamWriter->writeAttribute("i",QString::number(id_));
	DataUnit::serializeDataID(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //StateDataUnit

	return true;
}
//! Converts XML into a StateDataUnit object.  Note that this deletes any existing data.
bool StateDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
 	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "SDU")
	{
		addError("StateDataUnit","Incorrect tag, expected <SDU>",xmlStreamReader);
		return false;
	}

	actionFrame_ = xmlStreamReader->attributes().value("f").toString().toLongLong();
	id_ = xmlStreamReader->attributes().value("i").toString().toInt();
	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "SDU") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		DataUnit::deserializeDataID(xmlStreamReader);
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("StateDataUnit", "Unexpected end of document", xmlStreamReader);
		return false;
	}
	return true;
}

void StateDataUnit::postDeserialize()
{

}

bool StateDataUnit::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
