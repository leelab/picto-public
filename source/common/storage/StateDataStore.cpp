#include "StateDataStore.h"

namespace Picto {

StateDataStore::StateDataStore()
{
}

void StateDataStore::setTransition(QSharedPointer<Transition> transition, double timestamp, QString stateMachinePath)
{
	transition_ = transition; 
	timestamp_ = timestamp;
	machinePath_ = stateMachinePath;
}
void StateDataStore::setTransition(QString source, QString sourceResult, QString destination, double timestamp, QString stateMachinePath)
{
	transition_ = QSharedPointer<Transition>(new Transition(source, sourceResult, destination));
	timestamp_ = timestamp;
	machinePath_ = stateMachinePath;
}

/*! \brief Turns the StateDataStore into an XML fragment
 *
 *	The XML will look like this:
 *	<StateDataStore timestamp = 123.456 statemachinepath="state machine name">
 *		<Transition>
 *			<Source>TestState</Source>
 *			<SourceResult>Success</SourceResult>
 *			<Destination>Done</Destination>
 *		</Transition>
 *	</StateDataStore>
 */
bool StateDataStore::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("StateDataStore");
	xmlStreamWriter->writeAttribute("timestamp",QString::number(timestamp_));
	xmlStreamWriter->writeAttribute("statemachinepath",machinePath_);

	if(transition_)
		transition_->toXml(xmlStreamWriter);
	DataStore::serializeDataID(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //StateDataStore

	return true;
}
//! Converts XML into a StateDataStore object.  Note that this deletes any existing data.
bool StateDataStore::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
 	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateDataStore")
	{
		addError("StateDataStore","Incorrect tag, expected <StateDataStore>",xmlStreamReader);
		return false;
	}

	timestamp_ = xmlStreamReader->attributes().value("timestamp").toString().toDouble();
	machinePath_ = xmlStreamReader->attributes().value("statemachinepath").toString();
	transition_ = QSharedPointer<Transition>(new Transition());

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateDataStore") && !xmlStreamReader->atEnd())
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
				addError("StateDataStore", "Failed to deserialize Transition element", xmlStreamReader);
				return false;
			}
		}
		else
		{
			DataStore::deserializeDataID(xmlStreamReader);
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("StateDataStore", "Unexpected end of document", xmlStreamReader);
		return false;
	}
	return true;
}

bool StateDataStore::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
