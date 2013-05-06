#include "Parameter.h"
#include "../memleakdetect.h"

namespace Picto {

Parameter::Parameter() :
	fixingValues_(false)
{
}

//void Parameter::reset()
//{
//	setValue(propertyContainer_->getPropertyValue("Value"));
//}

//void Parameter::setInitialValue(QVariant val)
//{
//	propertyContainer_->setPropertyValue("Value",verifyValue(val));
//	//PUT IN VERIFY VALUE BUT WITH INPUT VALUE SO THAT THIS CAN BE VERIFIED TO!!!
//}

//void Parameter::setValue(QVariant value)
//{
//	propertyContainer_->setPropertyValue("Value",verifyValue(value));
//}
//
//QVariant Parameter::getValue()
//{
//	return propertyContainer_->getPropertyValue("Value");
//}
bool Parameter::valuesAreValid(QString& warning)
{
	return true;
}

void Parameter::fixValues()
{
}


void Parameter::postDeserialize()
{
	Scriptable::postDeserialize();
}

bool Parameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Scriptable::validateObject(xmlStreamReader))
		return false;
	QString warning;
	if(!valuesAreValid(warning))
	{
		if(warning.isEmpty())
			warning = "Invalid property value detected.";
		addError(warning);
		return false;
	}
	return true;
}

}; //namespace Picto
