#include "RangeParameter.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new RangeParameter
 *	\details Adds a number of Properties
 *		- Value - The integer value stored by this RangeParameter
 *		- Min - The minimum allowable Value
 *		- Max - The maximum allowable Value
 *		- Increment - The incremement from one allowable value to the next.  If this is 2 for example and
 *			Min is 0 with Max being 10, allowable values will be 0,2,4,6.8.10.
 *		\note Since Increment was not too useful and pretty confusing, we have removed it from the list of 
 *		visible parameters.  It has not been removed though to support older session designs.
*/
RangeParameter::RangeParameter()
: Parameter()
{
	AddDefinableProperty(QVariant::Int,"Value",QVariant());
	AddDefinableProperty(QVariant::Int,"Min",0);
	AddDefinableProperty(QVariant::Int,"Max",0);
	AddDefinableProperty(QVariant::Int,"Increment",0);
	AddDefinableObjectFactory("Units",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
}

/*! \brief The NewParameter is not used anymore by anything except the obsolete EngineTest.  It should be removed.
 *	Create() is now the function to use.
 */
Parameter* RangeParameter::NewParameter()
{
	return new RangeParameter;
}

/*! \brief Creates a new RangeParameter and returns a shared Asset pointer to it.
*/
QSharedPointer<Asset> RangeParameter::Create()
{
	return QSharedPointer<Asset>(new RangeParameter());
}

/*! \brief Extends Parameter::postDeserialize() to set the Increment Property invisible.  
 *	\details It was confusing and not very useful but we didn't want to break old designs
 *	so we've kept it around but made it invisible so it won't be used in new Designs.
 */
 void RangeParameter::postDeserialize()
{
	Parameter::postDeserialize();
	propertyContainer_->getProperty("Increment")->setVisible(false);
	setPropertyRuntimeEditable("Value");
}

bool RangeParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

/*! \brief Extends Parameter::valuesAreValid() to verify that the Value is between Min
 *	and Max, and that Min is not greatner than Max.
 *	\note To support older experiments, Increment is still checked such that if it
 *	is not zero, values must be a precise number of Increment intervals from Min.
 */
bool RangeParameter::valuesAreValid(QString& warning)
{
	int min = propertyContainer_->getProperty("Min")->initValue().toInt();
	int max = propertyContainer_->getProperty("Max")->initValue().toInt();
	int inc = propertyContainer_->getProperty("Increment")->initValue().toInt();
	int value = propertyContainer_->getProperty("Value")->initValue().toInt();
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

	//If increment is less than or equal to zero, it is set to zero and ignored.
	if(inc < 0)
	{
		warning = "'Increment' must be greater than or equal to zero.";
		return false;
	}

	int distFromMin = value - min;
	if(inc && (distFromMin%inc != 0)) 
	{
		warning = "If 'Increment' is used, 'Value' must be a precise 'Increment' above 'Min'.";
		return false;
	}
	return true;
}

/*! \brief Extends Parameter::fixValues() to keep min <= max and min <= value <= max.
 *	\note To support older experiments, Increment is still checked such that if it
 *	is not zero, values are changed to the closest value that is a precise interval of 
 *	Increments from Min.
 */
void RangeParameter::fixValues()
{
	int min = propertyContainer_->getProperty("Min")->initValue().toInt();
	int max = propertyContainer_->getProperty("Max")->initValue().toInt();
	int inc = propertyContainer_->getProperty("Increment")->initValue().toInt();
	int value = propertyContainer_->getProperty("Value")->initValue().toInt();
	if(min > max)
	{
		min = max;
		propertyContainer_->getProperty("Min")->setInitValue(min);
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

	//If increment is less than zero, set it to zero.
	if(inc < 0)
	{
		inc = 0;
		propertyContainer_->getProperty("Increment")->setInitValue(inc);
	}

	int distFromMin = inc - min;
	if(inc && (distFromMin%inc != 0)) 
	{
		//force the current value to be a multiple of the increment.  
		value = min + (distFromMin/inc)*inc;
		propertyContainer_->getProperty("Value")->setInitValue(value);
	}
}

}; //namespace Picto
