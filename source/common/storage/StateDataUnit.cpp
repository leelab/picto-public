#include "StateDataUnit.h"

namespace Picto {

StateDataUnit::StateDataUnit()
{
}

void StateDataUnit::setTransition(QSharedPointer<Transition> transition, double timestamp, QString stateMachinePath)
{
	transition_ = transition; 
	timestamp_ = timestamp;
	machinePath_ = stateMachinePath;
}
void StateDataUnit::setTransition(QString source, QString sourceResult, QString destination, double timestamp, QString stateMachinePath)
{
	transition_ = QSharedPointer<Transition>(new Transition(source, sourceResult, destination));
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
	xmlStreamWriter->writeAttribute("timestamp",QString::number(timestamp_,'e',6));
	xmlStreamWriter->writeAttribute("statemachinepath",machinePath_);

	if(transition_)
		transition_->toXml(xmlStreamWriter);
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

	timestamp_ = xmlStreamReader->attributes().value("timestamp").toString().toDouble();
	machinePath_ = xmlStreamReader->attributes().value("statemachinepath").toString();
	transition_ = QSharedPointer<Transition>(new Transition());

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateDataUnit") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Transition")
		{
			if(!transition_->fromXml(xmlStreamReader))
			{
				addError("StateDataUnit", "Failed to deserialize Transition element", xmlStreamReader);
				return false;
			}
		}
		else
		{
			DataUnit::deserializeDataID(xmlStreamReader);
		}
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
