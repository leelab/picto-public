#include "ControlElementFactory.h"

namespace Picto
{

QMap<QString, ControlElementFactory::NewControlElementFnPtr> ControlElementFactory::controlElements_;

ControlElementFactory::ControlElementFactory()
{
}

QSharedPointer<ControlElement> ControlElementFactory::generateControlElement(QString type)
{
	if(!controlElements_.contains(type))
		return QSharedPointer<ControlElement>();

	QSharedPointer<ControlElement> elem(controlElements_[type]());
	return elem;
}

void ControlElementFactory::addControlElementType(QString type, NewControlElementFnPtr newFnPtr)
{
	if(controlElements_.contains(type))
		return;

	controlElements_[type] = newFnPtr;
	return;
}

} //namespace Picto
