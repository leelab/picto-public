#include "StateDataUnit.h"

namespace Picto {

StateDataUnit::StateDataUnit()
{
}

void StateDataUnit::setTransition(QSharedPointer<Transition> transition, double timestamp, QString stateMachinePath)
{
	setTransition(transition->getSource(),transition->getSourceResult(),transition->getDestination(),QString("%1").arg(timestamp,0,'f',6),stateMachinePath);
}
void StateDataUnit::setTransition(QString source, QString sourceResult, QString destination, double timestamp, QString stateMachinePath)
{
	setTransition(source,sourceResult,destination,QString("%1").arg(timestamp,0,'f',6),stateMachinePath);
}
void StateDataUnit::setTransition(QString source, QString sourceResult, QString destination, QString timestamp, QString stateMachinePath)
{
	source_ = source;
	sourceResult_ = sourceResult;
	destination_ = destination;
	timestamp_ = timestamp;
	machinePath_ = stateMachinePath;
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
	xmlStreamWriter->writeStartElement("StateDataUnit");
	xmlStreamWriter->writeAttribute("timestamp",timestamp_);
	xmlStreamWriter->writeAttribute("statemachinepath",machinePath_);
	xmlStreamWriter->writeAttribute("src",source_);
	xmlStreamWriter->writeAttribute("srcRes",sourceResult_);
	xmlStreamWriter->writeAttribute("dest",destination_);
	DataUnit::serializeDataID(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //StateDataUnit

	return true;
}
//! Converts XML into a StateDataUnit object.  Note that this deletes any existing data.
bool StateDataUnit::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
 	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateDataUnit")
	{
		addError("StateDataUnit","Incorrect tag, expected <StateDataUnit>",xmlStreamReader);
		return false;
	}

	timestamp_ = xmlStreamReader->attributes().value("timestamp").toString();
	machinePath_ = xmlStreamReader->attributes().value("statemachinepath").toString();
	source_ = xmlStreamReader->attributes().value("src").toString();
	sourceResult_ = xmlStreamReader->attributes().value("srcRes").toString();
	destination_ = xmlStreamReader->attributes().value("dest").toString();
	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateDataUnit") && !xmlStreamReader->atEnd())
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

void StateDataUnit::postSerialize()
{

}

bool StateDataUnit::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
