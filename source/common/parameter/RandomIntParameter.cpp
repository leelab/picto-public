#include "RandomIntParameter.h"

namespace Picto
{

RandomIntParameter::RandomIntParameter()
: 
  value_(0),
  units_(""),
  Parameter()
{
	AddDefinableProperty(QVariant::Int,"Value",QVariant());
	AddDefinableProperty(QVariant::Bool,"UseSeed",false);
	AddDefinableProperty(QVariant::Int,"Seed",0);
	AddDefinableProperty(QVariant::Int,"Min",0);
	AddDefinableProperty(QVariant::Int,"Max",1);

}

Parameter* RandomIntParameter::NewParameter()
{
	return new RandomIntParameter;
}

void RandomIntParameter::randomize()
{
	checkForPropertyChanges();
	int min = propertyContainer_->getPropertyValue("Min").toInt();
	int max = propertyContainer_->getPropertyValue("Max").toInt();
	propertyContainer_->setPropertyValue("Value",QVariant(int(min+mtRand_.randInt(max-min))));
}

void RandomIntParameter::postDeserialize()
{
	Parameter::postDeserialize();
	useSeed_ = propertyContainer_->getPropertyValue("UseSeed").toBool();
	seed_ = propertyContainer_->getPropertyValue("Seed").toInt();
	if(useSeed_)
		mtRand_.seed(seed_);
	setPropertyRuntimeEditable("Value");
}

bool RandomIntParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{

	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	if(propertyContainer_->getPropertyValue("Min").toInt() > propertyContainer_->getPropertyValue("Max").toInt())
	{
		addError("RandomIntParameter", "Min value cannot be greater than Max value.", xmlStreamReader);
		return false;
	}
	return true;
}

bool RandomIntParameter::fixValues(QString&)
{
	bool returnVal = true;
	int min = propertyContainer_->getPropertyValue("Min").toInt();
	int max = propertyContainer_->getPropertyValue("Max").toInt();
	int value = propertyContainer_->getPropertyValue("Value").toInt();
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

void RandomIntParameter::checkForPropertyChanges()
{
	bool useSeed = propertyContainer_->getPropertyValue("UseSeed").toBool();
	if(useSeed)
	{
		int seed = propertyContainer_->getPropertyValue("Seed").toInt();
		if(!useSeed_ || (seed_ != seed))
		{
			mtRand_.seed(seed);
		}
		seed_ = seed;
	}
	useSeed_ = useSeed;
}

} //namespace Picto