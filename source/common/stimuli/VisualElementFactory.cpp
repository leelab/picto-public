#include "VisualElementFactory.h"
#include "../memleakdetect.h"

namespace Picto
{

QMap<QString, VisualElementFactory::NewVisualElementFnPtr> VisualElementFactory::visualElements_;

VisualElementFactory::VisualElementFactory()
{
}

QSharedPointer<VisualElement> VisualElementFactory::generateVisualElement(QString type)
{
	//If the visualElements_ list is empty, the factory was 
	//never initialized, so someome screwed up...
	Q_ASSERT(!visualElements_.isEmpty());

	if(!visualElements_.contains(type))
		return QSharedPointer<VisualElement>();

	QSharedPointer<VisualElement> v(visualElements_[type]());
	return v;
}

void VisualElementFactory::addVisualElementType(QString type, NewVisualElementFnPtr newFnPtr)
{
	if(visualElements_.contains(type))
		return;

	visualElements_[type] = newFnPtr;
}

} //namespace Picto
