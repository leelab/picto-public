#include "NumericParameter.h"

namespace Picto
{

NumericParameter::NumericParameter()
: 
  value_(0),
  units_("")
{
	AddDefinableProperty("Name","");\
	AddDefinableProperty(QVariant::Bool,"OperatorUI",false);
	AddDefinableProperty(QVariant::Int,"Order","");
	AddDefinableProperty("Units","");
	type_ = "Numeric";

}

Parameter* NumericParameter::NewParameter()
{
	return new NumericParameter;
}

void NumericParameter::setValue(QVariant value)
{
	bool ok;
	int testValue;
	testValue = value.toInt(&ok);
	if(ok)
		value_ = testValue;
}

//note that the lessThan & greaterThan functions aren't redefined, 
//so they will always return false
bool NumericParameter::greaterThan(Parameter& RHS)
{
	//first confirm that the Parameter is also numeric
	if(RHS.type() != "Numeric")
		return false;

	return value_ > RHS.getValue().toInt();
}
bool NumericParameter::equalTo(Parameter& RHS)
{
	//first confirm that the Parameter is also numeric
	if(RHS.type() != "Numeric")
		return false;

	return value_ == RHS.getValue().toInt();
}
bool NumericParameter::lessThan(Parameter& RHS)
{
	//first confirm that the Parameter is also numeric
	if(RHS.type() != "Numeric")
		return false;

	return value_ < RHS.getValue().toInt();
}

bool NumericParameter::greaterThan(QVariant& RHS)
{
	//first confirm that the Variant can be converted to an int
	int rhsValue;
	bool ok;
	rhsValue = RHS.toInt(&ok);
	if(!ok)
		return false;
	else
		return value_ > rhsValue;
}
bool NumericParameter::equalTo(QVariant& RHS)
{
	//first confirm that the Variant can be converted to an int
	int rhsValue;
	bool ok;
	rhsValue = RHS.toInt(&ok);
	if(!ok)
		return false;
	else
		return value_ == rhsValue;
}
bool NumericParameter::lessThan(QVariant& RHS)
{
	//first confirm that the Variant can be converted to an int
	int rhsValue;
	bool ok;
	rhsValue = RHS.toInt(&ok);
	if(!ok)
		return false;
	else
		return value_ < rhsValue;
}

/*! \brief Outputs a numericParameter to XML
 *
 *	The XML syntax is this:
 *		<Parameter operatorUI="true" type="Numeric">
 *	 		<Name>Trial Number</Name>
 *			<Order>3</Order>
  *			<Units>ms</Units>
*		</Parameter>
 *
 */
//bool NumericParameter::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("Parameter");
//	if(bOperatorUI_)
//		xmlStreamWriter->writeAttribute("operatorUI","true");
//	else
//		xmlStreamWriter->writeAttribute("operatorUI","false");
//
//	xmlStreamWriter->writeAttribute("type", type_);
//
//	xmlStreamWriter->writeTextElement("Name", name());
//	xmlStreamWriter->writeTextElement("Order", QString("%1").arg(order_));
//	if(!units_.isEmpty())
//		xmlStreamWriter->writeTextElement("Units",units_);
//	return false;
//}
//
///*	\brief Converts an XML fragment into a NumericParameter
// *	
// *	The XML fragment will look like this:
// *		<Parameter operatorUI="true" type="Numeric">
// *	 		<Name>Trial Number</Name>
// *			<Order>3</Order>
// *			<Units>ms</Units>
// *		</Parameter>
// *
// */
//
//bool NumericParameter::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "Parameter")
//	{
//		addError("NumericParameter","Incorrect tag, expected <Parameter>",xmlStreamReader);
//		return false;
//	}
//	if(xmlStreamReader->attributes().value("type").toString() != type_)
//	{
//		addError("NumericParameter","Incorrect type of parameter",xmlStreamReader);
//		return false;
//	}
//
//	//grab the operatorUI attribute
//	QString operatorUIStr = xmlStreamReader->attributes().value("operatorUI").toString();
//
//	if(operatorUIStr == "true")
//		bOperatorUI_ = true;
//	else
//		bOperatorUI_ = false;
//
//	//loop through everything else
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Parameter") && !xmlStreamReader->atEnd())
//	{
//		//do nothing unless we're looking at a start element
//		if(!xmlStreamReader->isStartElement())
//		{
//			xmlStreamReader->readNext();
//			continue;
//		}
//		QString name = xmlStreamReader->name().toString();
//
//		//LHS parameter
//		if(name == "Name")
//		{
//			name_ = xmlStreamReader->readElementText();
//		}
//		else if(name == "Order")
//		{
//			order_ = xmlStreamReader->readElementText().toInt();
//		}
//		else if(name == "Units")
//		{
//			units_ = xmlStreamReader->readElementText();
//		}
//		else
//		{
//			addError("NumericParameter", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//		xmlStreamReader->readNext();
//	}
//
//	if(xmlStreamReader->atEnd())
//	{
//		addError("NumericParameter", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//	return true;
//}

bool NumericParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	bOperatorUI_ = propertyContainer_->getPropertyValue("OperatorUI").toBool();
	name_ = propertyContainer_->getPropertyValue("Name").toString();
	order_ = propertyContainer_->getPropertyValue("Order").toInt();
	units_ = propertyContainer_->getPropertyValue("Units").toString();
	return true;
}

} //namespace Picto