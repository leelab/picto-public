#include "RandomNormDoubleParameter.h"
#include "../memleakdetect.h"

namespace Picto
{

/*! \brief Constructs a new RandomNormDoubleParameter
 *	\details Adds a number of Properties
 *		- Value - The random value last generated with randomize().
 *		- UseSeed - Sets whether a seed is used to reproduce the random series of values predictably.
 *		- Seed - The seed that will be used if "UseSeed" is true.
 *		- Min - The minimum double that will be randomizable
 *		- Max - The maximum double that will be randomizable
*/
RandomNormDoubleParameter::RandomNormDoubleParameter()
: 
  value_(0),
  units_(""),
  Parameter()
{
	AddDefinableProperty(QVariant::Double,"Value",QVariant());
	AddDefinableProperty(QVariant::Bool,"UseSeed",false);
	AddDefinableProperty(QVariant::Int,"Seed",0);
	AddDefinableProperty(QVariant::Double, "Mean", 0.0);
	AddDefinableProperty(QVariant::Double, "StdDeviation", 1.0);

}

/*! \brief The NewParameter is not used anymore by anything except the obsolete EngineTest.  It should be removed.
 *	Create() is now the function to use.
 */
Parameter* RandomNormDoubleParameter::NewParameter()
{
	return new RandomNormDoubleParameter;
}

/*! \brief Creates a new RandomNormDoubleParameter and returns a shared Asset pointer to it.
*/
QSharedPointer<Asset> RandomNormDoubleParameter::Create()
{
	return QSharedPointer<Asset>(new RandomNormDoubleParameter());
}

/*! \brief Causes a new random value between the Min (>=) and Max (<) Property values to be set to the
 *	Value Property.  It will then be readable using the "value" script property.
 */
void RandomNormDoubleParameter::randomize()
{
	checkForPropertyChanges();
		
	double mean = propertyContainer_->getPropertyValue("Mean").toDouble();
	double stdDev = propertyContainer_->getPropertyValue("StdDeviation").toDouble();

	
	double u1 = double(mtRand_.rand()); //uniform(0,1] random doubles
	double u2 = double(mtRand_.rand());

	double M_PI = 3.14159265358979323846;
	double randStdNormal = sqrt(-2.0 * log(u1)) * sin(2.0 * M_PI * u2); //random normal(0,1) - Box-Muller transform
	double randNormal =  mean + stdDev * randStdNormal; //random normal(mean,stdDev^2)
		
	//double randNormal = mtRand_.randNorm(mean, stdDev); //random normal(mean,stdDev^2)

	propertyContainer_->setPropertyValue("Value",QVariant(double(randNormal)));

}

void RandomNormDoubleParameter::postDeserialize()
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
bool RandomNormDoubleParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{

	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	double stdDev = propertyContainer_->getProperty("StdDeviation")->initValue().toDouble();

	if (stdDev < 0)
	{
		addError("Standard Deviation value cannot be a negative value.");
		return false;
	}
	return true;
}

bool RandomNormDoubleParameter::valuesAreValid(QString& warning)
{	
	double stdDev = propertyContainer_->getProperty("StdDeviation")->initValue().toDouble();

	if (stdDev < 0)
	{
		warning = "'Standard Deviation' value must be positive.";
		return false;
	}

	return true;
}

void RandomNormDoubleParameter::fixValues()
{
	double stdDev = propertyContainer_->getProperty("StdDeviation")->initValue().toDouble();

	if (stdDev < 0)
	{
		double oppStdDev = -1.0*stdDev;
		propertyContainer_->getProperty("StdDeviation")->setInitValue(oppStdDev);
	}

	return;
}

/*! \brief Checks if this object's "UseSeed" or "Seed" Property values changed.  If so, the seed
 *	is updated.
*/
void RandomNormDoubleParameter::checkForPropertyChanges()
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