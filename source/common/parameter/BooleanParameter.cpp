#include "BooleanParameter.h"

namespace Picto {

BooleanParameter::BooleanParameter()
: Parameter()
{
	AddDefinableProperty(QVariant::Bool,"Value",QVariant());
	AddDefinableProperty("TrueLabel","True");
	AddDefinableProperty("FalseLabel","False");
	trueLabel_ = "";
	falseLabel_ = "";
}

Parameter* BooleanParameter::NewParameter()
{
	return new BooleanParameter;
}

void BooleanParameter::postDeserialize()
{
	Parameter::postDeserialize();
	trueLabel_ = propertyContainer_->getPropertyValue("TrueLabel").toString();
	falseLabel_ = propertyContainer_->getPropertyValue("FalseLabel").toString();
	setPropertyRuntimeEditable("Value");
}

bool BooleanParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
