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
		addError("Min value cannot be greater than Max value.");
		return false;
	}
	return true;
}

bool DoubleParameter::valuesAreValid(QString& warning)
{
	double min = propertyContainer_->getProperty("Min")->initValue().toDouble();
	double max = propertyContainer_->getProperty("Max")->initValue().toDouble();
	double value = propertyContainer_->getProperty("Value")->initValue().toDouble();
	if(min > max)
	{
		warning = "'Min' value must be less than or equal to 'Max' value.";
		return false;
	}

	if(value < min)
	{
		warning = "'Value' is less than 'Min'.";
		return false;
	}
	if(value > max)
	{
		warning = "'Value' is greater than than 'Max'.";
		return false;
	}
	return true;
}

void DoubleParameter::fixValues()
{
	double min = propertyContainer_->getProperty("Min")->initValue().toDouble();
	double max = propertyContainer_->getProperty("Max")->initValue().toDouble();
	double value = propertyContainer_->getProperty("Value")->initValue().toDouble();
	if(min > max)
	{
		min = max;
		propertyContainer_->getProperty("Min")->setInitValue(min);
		propertyContainer_->getProperty("Value")->setInitValue(min);
	}
	if(value < min)
	{
		value = min;
		propertyContainer_->getProperty("Value")->setInitValue(value);
	}
	if(value > max)
	{
		value = max;
		propertyContainer_->getProperty("Value")->setInitValue(value);
	}
}

void DoubleParameter::checkForPropertyChanges()
{
}

} //namespace Picto