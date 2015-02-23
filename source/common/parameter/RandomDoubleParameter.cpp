#include "RandomDoubleParameter.h"
#include "../memleakdetect.h"

namespace Picto
{

/*! \brief Constructs a new RandomDoubleParameter
 *	\details Adds a number of Properties
 *		- Value - The random value last generated with randomize().
 *		- UseSeed - Sets whether a seed is used to reproduce the random series of values predictably.
 *		- Seed - The seed that will be used if "UseSeed" is true.
 *		- Min - The minimum double that will be randomizable
 *		- Max - The maximum double that will be randomizable
*/
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

/*! \brief The NewParameter is not used anymore by anything except the obsolete EngineTest.  It should be removed.
 *	Create() is now the function to use.
 */
Parameter* RandomDoubleParameter::NewParameter()
{
	return new RandomDoubleParameter;
}

/*! \brief Creates a new RandomDoubleParameter and returns a shared Asset pointer to it.
*/
QSharedPointer<Asset> RandomDoubleParameter::Create()
{
	return QSharedPointer<Asset>(new RandomDoubleParameter());
}

/*! \brief Causes a new random value between the Min (>=) and Max (<) Property values to be set to the
 *	Value Property.  It will then be readable using the "value" script property.
 */
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

/*! \brief Extends Parameter::validateObject() to verify that the Min Property is not higher
 *	than the Max Property.
 */
bool RandomDoubleParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
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

bool RandomDoubleParameter::valuesAreValid(QString& warning)
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

void RandomDoubleParameter::fixValues()
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

/*! \brief Checks if this object's "UseSeed" or "Seed" Property values changed.  If so, the seed
 *	is updated.
*/
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