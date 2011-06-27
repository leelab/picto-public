#include "RangeParameter.h"

namespace Picto {

RangeParameter::RangeParameter()
{
	
	AddDefinableProperty(QVariant::Int,"Order",0);
	AddDefinableProperty(QVariant::Int,"Default",0);
	AddDefinableProperty(QVariant::Int,"Min",0);
	AddDefinableProperty(QVariant::Int,"Max",0);
	AddDefinableProperty(QVariant::Int,"Increment",0);
	AddDefinableProperty("Units","");
	type_ = "Range";
	increment_ = 1;
}

Parameter* RangeParameter::NewParameter()
{
	return new RangeParameter;
}

/*! Note that setValue sets the value to the closest value that is 
 * a multiple of the increment
 */
void RangeParameter::setValue(QVariant value)
{
	if(value.toInt() < minValue_)
		currentValue_ = minValue_;
	else if(value.toInt() > maxValue_)
		currentValue_ = maxValue_;
	else
	{
		currentValue_ = value.toInt();
	
		//force the current value to be a multiple of the increment
		currentValue_ = (currentValue_/increment_)*increment_;

	}
}

void RangeParameter::setMin(int min) 
{
	//check that min < max
	if(min <= maxValue_) 
		minValue_ = min; 

	//force the min value to be a multiple of the increment
	minValue_ = (minValue_/increment_)*increment_;
}
void RangeParameter::setMax(int max) 
{ 
	//check that max > min
	if(max >= minValue_) 
		maxValue_ = max; 

	//force the max value to be a multiple of the increment
	maxValue_ = (maxValue_/increment_)*increment_;
}

void RangeParameter::setDefault(int defualtValue) 
{ 
	if(defualtValue <= maxValue_ && defualtValue >= minValue_) 
	{
		defaultValue_ = defualtValue; 
		//force the default value to be a multiple of the increment
		defaultValue_ = (defaultValue_/increment_)*increment_;
		currentValue_ = defaultValue_;
	}
}

void RangeParameter::postSerialize()
{
	Parameter::postSerialize();
	bOperatorUI_ = propertyContainer_->getPropertyValue("OperatorUI").toBool();
	order_ = propertyContainer_->getPropertyValue("Order").toInt();
	defaultValue_ = propertyContainer_->getPropertyValue("Default").toInt();
	minValue_ = propertyContainer_->getPropertyValue("Min").toInt();
	maxValue_ = propertyContainer_->getPropertyValue("Max").toInt();
	increment_ = propertyContainer_->getPropertyValue("Increment").toInt();
	units_ = propertyContainer_->getPropertyValue("Units").toString();
}

bool RangeParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
