#include "RandomDoubleParameter.h"

namespace Picto
{

RandomDoubleParameter::RandomDoubleParameter()
: 
  value_(0),
  units_(""),
  Parameter()
{
	AddDefinableProperty(QVariant::Double,"Value",QVariant());
	AddDefinableProperty(QVariant::Bool,"UseSeed",false);
	AddDefinableProperty(QVariant::Int,"Seed",0);
	AddDefinableProperty(QVariant::Double,"Min",0.0);
	AddDefinableProperty(QVariant::Double,"Max",1.0);

}

Parameter* RandomDoubleParameter::NewParameter()
{
	return new RandomDoubleParameter;
}

void RandomDoubleParameter::randomize()
{
	checkForPropertyChanges();
	double min = propertyContainer_->getPropertyValue("Min").toDouble();
	double max = propertyContainer_->getPropertyValue("Max").toDouble();
	propertyContainer_->setPropertyValue("Value",QVariant(double(min+mtRand_.rand(max-min))));
}

void RandomDoubleParameter::postDeserialize()
{
	Parameter::postDeserialize();
	useSeed_ = propertyContainer_->getPropertyValue("UseSeed").toBool();
	seed_ = propertyContainer_->getPropertyValue("Seed").toInt();
	if(useSeed_)
		mtRand_.seed(seed_);
	setPropertyRuntimeEditable("Value");
}

bool RandomDoubleParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{

	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	if(propertyContainer_->getPropertyValue("Min").toDouble() > propertyContainer_->getPropertyValue("Max").toDouble())
	{
		addError("RandomDoubleParameter", "Min value cannot be greater than Max value.", xmlStreamReader);
		return false;
	}
	return true;
}

bool RandomDoubleParameter::fixValues(QString&)
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

void RandomDoubleParameter::checkForPropertyChanges()
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