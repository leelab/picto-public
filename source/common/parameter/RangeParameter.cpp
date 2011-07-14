#include "RangeParameter.h"

namespace Picto {

RangeParameter::RangeParameter()
: Parameter(QVariant::Int)
{
	
	AddDefinableProperty(QVariant::Int,"Order",0);
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

void RangeParameter::postSerialize()
{
	Parameter::postSerialize();
	bOperatorUI_ = propertyContainer_->getPropertyValue("OperatorUI").toBool();
	order_ = propertyContainer_->getPropertyValue("Order").toInt();
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

/*! Note that values should be set to the closest value that is 
 * a multiple of the increment
 */
QVariant RangeParameter::verifyValue(QVariant value)
{
	if(value.toInt() < minValue_)
		return minValue_;
	else if(value.toInt() > maxValue_)
		return maxValue_;
	else
	{
		//force the current value to be a multiple of the increment
		if(value.toInt() != (value.toInt()/increment_)*increment_)
			return QVariant((currentValue_/increment_)*increment_);
	}
	return value;
}

}; //namespace Picto
