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


}; //namespace Picto
