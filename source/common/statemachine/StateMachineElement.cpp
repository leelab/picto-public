#include "StateMachineElement.h"

namespace Picto {

StateMachineElement::StateMachineElement()
{
	initialElement_ = QUuid();
}

QPoint StateMachineElement::getDisplayLayoutPosition()
{
	return layoutPosition_;
}
/*!	\brief Adds a transition to this element
 *
 *	Note that to add a transition, the source and destination objects must already exist
 *	This will have an effect on the order in the XML.
 */
bool StateMachineElement::addTransition(QUuid source, QString condition, QUuid destination)
{
	//check that the source exists and has the correct type
	QSharedPointer<StateMachineElement> sourceElement;
	sourceElement = PictoIdentifiableObject::getObjectByID(source).staticCast<StateMachineElement>();
	if(sourceElement.isNull())
		return false;
	if(!validConatinedTypes_.contains(sourceElement->type()))
		return false;

	//check that the destination exists and has the correct type
	QSharedPointer<StateMachineElement> destElement;
	destElement = PictoIdentifiableObject::getObjectByID(source).staticCast<StateMachineElement>();
	if(destElement.isNull())
		return false;
	if(!validConatinedTypes_.contains(destElement->type()))
		return false;

	//Note that we use the [] operator to insert a source if one doesn't already exist
	transitions_[source].insert(condition,destination);
	return true;
}

/*	\brief Sets a default transition from source to destination
 *
 *	In the event that the string value returned by the source's run function
 *	doesn't match any of the transition conditions, the default transition
 *	will be used.
 */
bool StateMachineElement::setDefaultTransition(QUuid source, QUuid destination)
{
	//check that the source exists and has the correct type
	QSharedPointer<StateMachineElement> sourceElement;
	sourceElement = PictoIdentifiableObject::getObjectByID(source).staticCast<StateMachineElement>();
	if(sourceElement.isNull())
		return false;
	if(!validConatinedTypes_.contains(sourceElement->type()))
		return false;

	//check that the destination exists and has the correct type
	QSharedPointer<StateMachineElement> destElement;
	destElement = PictoIdentifiableObject::getObjectByID(source).staticCast<StateMachineElement>();
	if(destElement.isNull())
		return false;
	if(!validConatinedTypes_.contains(destElement->type()))
		return false;

	defaultTransitions_.insert(source, destination);
	return true;
}

bool StateMachineElement::setInitialElement(QUuid id)
{
	//check that the id exists and has a legal type
	QSharedPointer<StateMachineElement> element;
	element = PictoIdentifiableObject::getObjectByID(id).staticCast<StateMachineElement>();
	if(element.isNull())
		return false;
	if(!validConatinedTypes_.contains(element->type()))
		return false;
	
	initialElement_ = id;
	return true;
}

/*!	\brief Turns transitions map into XML
 *
 *	The transitions map will look like this after serialization:
 *	<Transitions>
 *		<Transition>
 *			<Source>{19C122D6-ECCC-43ee-8337-565A66944DE4}</Source>
 *			<Condition>Success</Condition>
 *			<Destination>{7d065af0-216b-11df-8a39-0800200c9a66}</Destination> 
 *		</Transition>
 *		<Transition>
 *			<Source>{19C122D6-ECCC-43ee-8337-565A66944DE4}</Source>
 *			<Condition>BrokeFixation</Condition>
 *			<Destination>{4def3f0-216b-11df-8a39-0800200c9a66}</Destination> 
 *		</Transition>
 *	</Transitions>
 */
bool StateMachineElement::serializeTransitions(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Transitions");

	QMap<QUuid, QMap<QString, QUuid> >::const_iterator src = transitions_.constBegin();
	while (src != transitions_.constEnd()) 
	{
		QString sourceStr = src.key().toString();

		QMap<QString, QUuid>::const_iterator cond = src.value().constBegin();
		while (cond != src.value().constEnd()) 
		{
			QString condStr = cond.key();
			QString destStr = cond.value().toString();

			xmlStreamWriter->writeStartElement("Transition");
			xmlStreamWriter->writeTextElement("Source",sourceStr);
			xmlStreamWriter->writeTextElement("Condition",sourceStr);
			xmlStreamWriter->writeTextElement("Destination",sourceStr);
			xmlStreamWriter->writeEndElement();
			cond++;
		}


		src++;
	}

	xmlStreamWriter->writeEndElement(); //Transitions;

	return true;
}

bool StateMachineElement::deserializeTransitions(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement())
		return false;
	if(xmlStreamReader->name() != "Transitions")
		return false;

	//clear out all existing transitions
	transitions_.clear();

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Transitions") && !xmlStreamReader->atEnd())
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
			QUuid source;
			QUuid dest;
			QString condition;

			bool initSrc = false;
			bool initDest = false;
			bool initCond = false;

			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Transition") && !xmlStreamReader->atEnd())
			{
				if(!xmlStreamReader->isStartElement())
				{
					//Do nothing unless we're at a start element
					xmlStreamReader->readNext();
					continue;
				}

				name = xmlStreamReader->name().toString();
				if(name == "Source")
				{
					QString sourceStr = xmlStreamReader->readElementText();
					source = QUuid(sourceStr);
					initSrc = true;
				}
				else if(name == "Condition")
				{
					condition = xmlStreamReader->readElementText();
					initCond = true;
				}
				else if(name == "Destination")
				{
					QString destStr = xmlStreamReader->readElementText();
					dest = QUuid(destStr);
					initDest = true;
				}
				else
				{
					return false;
				}
			}

			//if we're at the end, return false
			if(xmlStreamReader->atEnd())
				return false;

			//if stuff didn't get initialized, return false
			if(!(initSrc && initDest && initCond))
				return false;

			//add the new transition
			if(!addTransition(source, condition, dest))
				return false;
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
