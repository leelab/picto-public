#include "BooleanParameter.h"

namespace Picto {

BooleanParameter::BooleanParameter()
{
	
	AddDefinableProperty(QVariant::Bool,"OperatorUI",false);
	AddDefinableProperty(QVariant::Int,"Order",0);
	AddDefinableProperty(QVariant::Bool,"Default",0);
	AddDefinableProperty("TrueLabel","True");
	AddDefinableProperty("FalseLabel","False");
	type_ = "Boolean";
	trueLabel_ = "";
	falseLabel_ = "";
	defaultValue_ = false;
	value_ = defaultValue_;
}

Parameter* BooleanParameter::NewParameter()
{
	return new BooleanParameter;
}

void BooleanParameter::postSerialize()
{
	Parameter::postSerialize();
	bOperatorUI_ = propertyContainer_->getPropertyValue("OperatorUI").toBool();
	order_ = propertyContainer_->getPropertyValue("Order").toInt();
	defaultValue_ = propertyContainer_->getPropertyValue("Default").toBool();
	trueLabel_ = propertyContainer_->getPropertyValue("TrueLabel").toString();
	falseLabel_ = propertyContainer_->getPropertyValue("FalseLabel").toString();
}

bool BooleanParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
