#include "Stage.h"

namespace Picto {

Stage::Stage()
{
	type_ = "Stage";

	//Stages can contain states, results, flow elements, and 
	//other stages
	validConatinedTypes_.push_back("State");
	validConatinedTypes_.push_back("Stage");
	validConatinedTypes_.push_back("Result");
	validConatinedTypes_.push_back("FlowElement");
}

QString Stage::run()
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


/*	\brief Converts this Stage into an XML fragment
 *
 *	The Stage will look something like this when converted to XML:
		<Stage>
			<ID>8C4A5952-4C11-4324-ADA1-6E68BCB7B448</ID>
			<Name>Fixation</Name>
			<InitialElement>19C122D6-ECCC-43ee-8337-565A66944DE4</InitialElement>
			<Transitions>
				<Transition>
					...
				</Transition>
				<Transition>
					...
				</Transition>
			</Transitions>
		</Stage>
 */
bool Stage::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Stage");

	xmlStreamWriter->writeTextElement("ID",getId().toString());
	xmlStreamWriter->writeTextElement("Name",name_);
	xmlStreamWriter->writeTextElement("InitialElement", initialElement_.toString());

	serializeTransitions(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //Stage
	
	return true;
}
/*	\brief Converts an XML fragment into this Stage
 *
 *	The XML fragment will look something like this when:
		<Stage>
			<ID>8C4A5952-4C11-4324-ADA1-6E68BCB7B448</ID>
			<Name>Fixation</Name>
			<InitialElement>19C122D6-ECCC-43ee-8337-565A66944DE4</InitialElement>
			<Transitions>
				<Transition>
					...
				</Transition>
				<Transition>
					...
				</Transition>
			</Transitions>
		</Stage>
 */
bool Stage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement())
		return false;
	if(xmlStreamReader->name() != "Stage")
		return false;

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Stage") && !xmlStreamReader->atEnd())
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
