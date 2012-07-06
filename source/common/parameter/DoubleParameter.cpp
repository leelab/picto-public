#include "DoubleParameter.h"
#include "../memleakdetect.h"

namespace Picto
{

DoubleParameter::DoubleParameter()
: 
  Parameter()
{
	AddDefinableProperty(QVariant::Double,"Value",QVariant());
	AddDefinableProperty(QVariant::Double,"Min",0.0);
	AddDefinableProperty(QVariant::Double,"Max",1.0);

}

Parameter* DoubleParameter::NewParameter()
{
	return new DoubleParameter;
}

QSharedPointer<Asset> DoubleParameter::Create()
{
	return QSharedPointer<Asset>(new DoubleParameter());
}

double DoubleParameter::getValue()
{
	return propertyContainer_->getPropertyValue("Value").toDouble();
}

void DoubleParameter::setValue(double val)
{
	propertyContainer_->setPropertyValue("Value",val);
}

void DoubleParameter::postDeserialize()
{
	Parameter::postDeserialize();
	setPropertyRuntimeEditable("Value");
}

bool DoubleParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	if(propertyContainer_->getPropertyValue("Min").toDouble() > propertyContainer_->getPropertyValue("Max").toDouble())
	{
		addError("DoubleParameter", "Min value cannot be greater than Max value.", xmlStreamReader);
		return false;
	}
	return true;
}

bool DoubleParameter::fixValues(QString&)
{
	bool returnVal = true;
	double min = propertyContainer_->getPropertyValue("Min").toDouble();
	double max = propertyContainer_->getPropertyValue("Max").toDouble();
	double value = propertyContainer_->getPropertyValue("Value").toDouble();
	if(min > max)
	{
		min = max;
		propertyContainer_->setPropertyValue("Min",min);
		returnVal = false;
	}

	if(value < min)
	{
		value = min;
		propertyContainer_->setPropertyValue("Value",value);
		returnVal = false;
	}
	if(value > max)
	{
		value = max;
		propertyContainer_->setPropertyValue("Value",value);
		returnVal = false;
	}
	return returnVal;
}

void DoubleParameter::checkForPropertyChanges()
{
}

} //namespace Picto