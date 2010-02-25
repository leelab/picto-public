#include "Trial.h"

namespace Picto {

Trial::Trial()
{
	type_ = "Trial";

	//Trials can contain stages, results, and flow elements
	validConatinedTypes_.push_back("Stage");
	validConatinedTypes_.push_back("Result");
	validConatinedTypes_.push_back("FlowElement");
}

QString Trial::run()
{
	QUuid currId;
	QUuid nextId;
	QSharedPointer<StateMachineElement> currElement;
	QSharedPointer<StateMachineElement> nextElement;

	currId = initialElement_;
	
	while(true)
	{
		currElement = PictoIdentifiableObject::getObjectByID(currId).staticCast<StateMachineElement>();
		QString result = currElement->run();

		if(transitions_.contains(currId) && transitions_.value(currId).contains(result))
			nextId = transitions_.value(currId).value(result);
		else
			nextId = defaultTransitions_.value(currId);

		nextElement = PictoIdentifiableObject::getObjectByID(nextId).staticCast<StateMachineElement>();
		Q_ASSERT(nextElement);
		if(nextElement->type() == "Result")
			return nextElement->run();
		else
			currElement = nextElement;
	}
}


/*	\brief Converts this Trial into an XML fragment
 *
 *	The Trial will look something like this when converted to XML:
	<Trial>
		<ID>{26ccce30-223d-11df-8a39-0800200c9a66}</ID>
		<Name>ChoiceTrial</Name>
		<InitialElement>B6C5E483-41C3-4d2a-8DF4-CD9D12C38EB3"</InitialElement>
		<Transitions>
			...
		</Transitions>
	</Trial>
 */
bool Trial::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Trial");

	xmlStreamWriter->writeTextElement("ID",getId().toString());
	xmlStreamWriter->writeTextElement("Name",name_);
	xmlStreamWriter->writeTextElement("InitialElement", initialElement_.toString());

	serializeTransitions(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //Stage
	
	return true;
}
/*	\brief Converts an XML fragment into this Trial
 *
 *	The XML fragment will look something like this when:
 *	<Trial>
 *		<ID>{26ccce30-223d-11df-8a39-0800200c9a66}</ID>
 *		<Name>ChoiceTrial</Name>
 *		<InitialElement>B6C5E483-41C3-4d2a-8DF4-CD9D12C38EB3"</InitialElement>
 *		<Transitions>
 *			...
 *		</Transitions>
 *	</Trial>
 */
bool Trial::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement())
		return false;
	if(xmlStreamReader->name() != "Trial")
		return false;

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Trial") && !xmlStreamReader->atEnd())
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
		else if(name == "Name")
		{
			name_ = xmlStreamReader->readElementText();
		}
		else if(name == "InitialElement")
		{
			QString initElemStr = xmlStreamReader->readElementText();
			initialElement_ = QUuid(initElemStr);
		}
		else if(name == "Transitions")
		{
			deserializeTransitions(xmlStreamReader);
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
