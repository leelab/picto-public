#include "RangeParameter.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

RangeParameter::RangeParameter()
: Parameter()
{
	AddDefinableProperty(QVariant::Int,"Value",QVariant());
	AddDefinableProperty(QVariant::Int,"Min",0);
	AddDefinableProperty(QVariant::Int,"Max",0);
	AddDefinableProperty(QVariant::Int,"Increment",0);
	AddDefinableObjectFactory("Units",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
}

Parameter* RangeParameter::NewParameter()
{
	return new RangeParameter;
}

QSharedPointer<Asset> RangeParameter::Create()
{
	return QSharedPointer<Asset>(new RangeParameter());
}

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

/*! Note that values should be set to the closest value that is 
 * a multiple of the increment
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
