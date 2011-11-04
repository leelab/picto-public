#include "NumericParameter.h"

namespace Picto
{

NumericParameter::NumericParameter()
: 
  value_(0),
  units_(""),
  Parameter()
{
	AddDefinableProperty(QVariant::Int,"Value",QVariant());
	AddDefinableProperty("Units","");

}

Parameter* NumericParameter::NewParameter()
{
	return new NumericParameter;
}

//void NumericParameter::setValue(QVariant value)
//{
//	bool ok;
//	int testValue;
//	testValue = value.toInt(&ok);
//	if(ok)
//		value_ = testValue;
//}

//note that the lessThan & greaterThan functions aren't redefined, 
//so they will always return false
//bool NumericParameter::greaterThan(Parameter& RHS)
//{
//	//first confirm that the Parameter is also numeric
//	if(RHS.type() != "Numeric")
//		return false;
//
//	return value_ > RHS.getValue().toInt();
//}
//bool NumericParameter::equalTo(Parameter& RHS)
//{
//	//first confirm that the Parameter is also numeric
//	if(RHS.type() != "Numeric")
//		return false;
//
//	return value_ == RHS.getValue().toInt();
//}
//bool NumericParameter::lessThan(Parameter& RHS)
//{
//	//first confirm that the Parameter is also numeric
//	if(RHS.type() != "Numeric")
//		return false;
//
//	return value_ < RHS.getValue().toInt();
//}
//
//bool NumericParameter::greaterThan(QVariant& RHS)
//{
//	//first confirm that the Variant can be converted to an int
//	int rhsValue;
//	bool ok;
//	rhsValue = RHS.toInt(&ok);
//	if(!ok)
//		return false;
//	else
//		return value_ > rhsValue;
//}
//bool NumericParameter::equalTo(QVariant& RHS)
//{
//	//first confirm that the Variant can be converted to an int
//	int rhsValue;
//	bool ok;
//	rhsValue = RHS.toInt(&ok);
//	if(!ok)
//		return false;
//	else
//		return value_ == rhsValue;
//}
//bool NumericParameter::lessThan(QVariant& RHS)
//{
//	//first confirm that the Variant can be converted to an int
//	int rhsValue;
//	bool ok;
//	rhsValue = RHS.toInt(&ok);
//	if(!ok)
//		return false;
//	else
//		return value_ < rhsValue;
//}

void NumericParameter::postDeserialize()
{
	Parameter::postDeserialize();
	units_ = propertyContainer_->getPropertyValue("Units").toString();
	setPropertyRuntimeEditable("Value");
}

bool NumericParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

} //namespace Picto