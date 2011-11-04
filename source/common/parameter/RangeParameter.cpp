#include "RangeParameter.h"

namespace Picto {

RangeParameter::RangeParameter()
: Parameter()
{
	AddDefinableProperty(QVariant::Int,"Value",QVariant());
	AddDefinableProperty(QVariant::Int,"Min",0);
	AddDefinableProperty(QVariant::Int,"Max",0);
	AddDefinableProperty(QVariant::Int,"Increment",0);
	AddDefinableProperty("Units","");
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

void RangeParameter::postDeserialize()
{
	Parameter::postDeserialize();
	updateFromProperties();
	setPropertyRuntimeEditable("Value");
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
bool RangeParameter::fixValues(QString&)
{
	updateFromProperties();
	bool returnVal = true;
	if(minValue_ > maxValue_)
	{
		minValue_ = maxValue_;
		propertyContainer_->setPropertyValue("Min",minValue_);
		returnVal = false;
	}

	if(increment_ > (maxValue_ - minValue_))
	{
		increment_ = maxValue_ - minValue_;
		propertyContainer_->setPropertyValue("Increment",increment_);
		returnVal = false;
	}

	if(value_ < minValue_)
	{
		value_ = minValue_;
		propertyContainer_->setPropertyValue("Value",value_);
		returnVal = false;
	}
	if(value_ > maxValue_)
	{
		value_ = maxValue_;
		propertyContainer_->setPropertyValue("Value",value_);
		returnVal = false;
	}

	//If increment is less than or equal to zero, it is set to zero and ignored.
	if(increment_ < 0)
	{
		increment_ = 0;
		propertyContainer_->setPropertyValue("Increment",increment_);
		returnVal = false;
	}

	if(increment_ > 0) 
	{//force the current value to be a multiple of the increment.  
		
		int distFromMin = value_ - minValue_;
		if(distFromMin != ((distFromMin/increment_)*increment_))
		{
			value_ = minValue_ + (distFromMin/increment_)*increment_;
			propertyContainer_->setPropertyValue("Value",value_);
			returnVal = false;
		}
	}
	return returnVal;
}

void RangeParameter::updateFromProperties()
{
	minValue_ = propertyContainer_->getPropertyValue("Min").toInt();
	maxValue_ = propertyContainer_->getPropertyValue("Max").toInt();
	increment_ = propertyContainer_->getPropertyValue("Increment").toInt();
	units_ = propertyContainer_->getPropertyValue("Units").toString();
	value_ = propertyContainer_->getPropertyValue("Value").toInt();
}

}; //namespace Picto
