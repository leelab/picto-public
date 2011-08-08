#include "RandomIntParameter.h"

namespace Picto
{

RandomIntParameter::RandomIntParameter()
: 
  value_(0),
  units_(""),
  Parameter(QVariant::Int)
{
	
	AddDefinableProperty(QVariant::Bool,"OperatorUI",false);
	AddDefinableProperty(QVariant::Bool,"UseSeed",false);
	AddDefinableProperty(QVariant::Int,"Seed",0);
	AddDefinableProperty(QVariant::Int,"Min",0);
	AddDefinableProperty(QVariant::Int,"Max",1);
	type_ = "Numeric";

}

Parameter* RandomIntParameter::NewParameter()
{
	return new RandomIntParameter;
}

QVariant RandomIntParameter::getLastValue()
{
	return Parameter::getValue();
}
QVariant RandomIntParameter::getValue()
{
	checkForPropertyChanges();
	int min = propertyContainer_->getPropertyValue("Min").toInt();
	int max = propertyContainer_->getPropertyValue("Max").toInt();
	currValue_ = QVariant(int(min+mtRand_.randInt(max-min)));
	return Parameter::getValue();
}
//void RandomIntParameter::setValue(QVariant value)
//{
//	bool ok;
//	int testValue;
//	testValue = value.toInt(&ok);
//	if(ok)
//		value_ = testValue;
//}

void RandomIntParameter::postSerialize()
{
	Parameter::postSerialize();
	bOperatorUI_ = propertyContainer_->getPropertyValue("OperatorUI").toBool();
	useSeed_ = propertyContainer_->getPropertyValue("UseSeed").toBool();
	seed_ = propertyContainer_->getPropertyValue("Seed").toInt();
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