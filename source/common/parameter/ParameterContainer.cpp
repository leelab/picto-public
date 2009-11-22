#include "ParameterContainer.h"

#include <QtVariantProperty>

namespace Picto {

ParameterContainer::ParameterContainer()
{
}

/*
void ParameterContainer::slotPropertyManagerValueChanged(QtProperty * property,
														 const QVariant & value)
{
	QMapIterator<QString, QtVariantProperty *> paramIterator(parameters_);
	while(paramIterator.hasNext())
	{
		paramIterator.next();
		if(paramIterator.value() == property)
		{
			emit signalParameterValueChanged(paramIterator.key(), value);
			break;
		}
	}
}
*/

}; //namespace Picto
