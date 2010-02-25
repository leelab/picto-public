#include "RangeParameter.h"

namespace Picto {

RangeParameter::RangeParameter()
{
	type_ = "Range";
	increment_ = 1;
}

Parameter* RangeParameter::NewParameter()
{
	return new RangeParameter;
}

/*! Note that setValue sets the value to the closest value that is 
 * a multiple of the increment
 */
void RangeParameter::setValue(QVariant value)
{
	if(value.toInt() < minValue_)
		currentValue_ = minValue_;
	else if(value.toInt() > maxValue_)
		currentValue_ = maxValue_;
	else
	{
		currentValue_ = value.toInt();
	
		//force the current value to be a multiple of the increment
		currentValue_ = (currentValue_/increment_)*increment_;

	}
}

void RangeParameter::setMin(int min) 
{
	//check that min < max
	if(min <= maxValue_) 
		minValue_ = min; 

	//force the min value to be a multiple of the increment
	minValue_ = (minValue_/increment_)*increment_;
}
void RangeParameter::setMax(int max) 
{ 
	//check that max > min
	if(max >= minValue_) 
		maxValue_ = max; 

	//force the max value to be a multiple of the increment
	maxValue_ = (maxValue_/increment_)*increment_;
}

void RangeParameter::setDefault(int defualtValue) 
{ 
	if(defualtValue <= maxValue_ && defualtValue >= minValue_) 
	{
		defaultValue_ = defualtValue; 
		//force the default value to be a multiple of the increment
		defaultValue_ = (defaultValue_/increment_)*increment_;
		currentValue_ = defaultValue_;
	}
}

/*! \brief Outputs a RangeParameter to XML
 *
 *	The XML syntax is this:
 *
 *	<Parameter operatorUI="true" type="Range"> 
 *		<Name>Maximum Fixation Wait Time</Name>
 *		<Order>1</Order>
 *		<Range default="5000">
 *			<Min>100</Min>
 *			<Max>10000</Max>
 *			<Increments>10</Increments>
 *			<Units>ms</Units>
 *		</Range>
 *	</Parameter>
 *
 */
bool RangeParameter::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Parameter");
	if(bOperatorUI_)
		xmlStreamWriter->writeAttribute("operatorUI","true");
	else
		xmlStreamWriter->writeAttribute("operatorUI","false");
	xmlStreamWriter->writeAttribute("type", type_);

	xmlStreamWriter->writeTextElement("Name", name());
	xmlStreamWriter->writeTextElement("Order", QString("%1").arg(order_));

	xmlStreamWriter->writeStartElement("Range");
	xmlStreamWriter->writeAttribute("default",QString("%1").arg(defaultValue_));

	xmlStreamWriter->writeTextElement("Min", QString("%1").arg(minValue_));
	xmlStreamWriter->writeTextElement("Max", QString("%1").arg(maxValue_));
	xmlStreamWriter->writeTextElement("Increment", QString("%1").arg(increment_));
	xmlStreamWriter->writeTextElement("Units", units_);


	xmlStreamWriter->writeEndElement();	//Range
	xmlStreamWriter->writeEndElement();	//Parameter

	return true;
}

/*! \brief Modifies this Rangearameter to match the passed in XML
 *
 *	The XML syntax is this:
 *
 *	<Parameter operatorUI="true" type="Range"> 
 *		<Name>Maximum Fixation Wait Time</Name>
 *		<Order>1</Order>
 *		<Range default="5000">
 *			<Min>100</Min>
 *			<Max>10000</Max>
 *			<Increments>10</Increments>
 *			<Units>ms</Units>
 *		</Range>
 *	</Parameter>
 *
 */
bool RangeParameter::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement())
		return false;
	if(xmlStreamReader->name() != "Parameter")
		return false;
	if(xmlStreamReader->attributes().value("type").toString() != type_)
		return false;

	//grab the operatorUI attribute
	QString operatorUIStr = xmlStreamReader->attributes().value("operatorUI").toString();

	if(operatorUIStr == "true")
		bOperatorUI_ = true;
	else
		bOperatorUI_ = false;

	//loop through everything else
	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Parameter"))
	{
		if(!xmlStreamReader->isStartElement())
		{
			//do nothing unless we're looking at a start element
		}
		else
		{
			//use the name to set up the sides and predicate.
			//Note that we assume the order is LHS, predicate, RHS
			QString name = xmlStreamReader->name().toString();

			//LHS parameter
			if(name == "Name")
			{
				name_ = xmlStreamReader->readElementText();
			}
			else if(name == "Order")
			{
				order_ = xmlStreamReader->readElementText().toInt();
			}
			else if(name == "Range")
			{
				defaultValue_ = xmlStreamReader->attributes().value("default").toString().toInt();
			}
			else if(name == "Min")
			{
				minValue_ = xmlStreamReader->readElementText().toInt();
			}
			else if(name == "Max")
			{
				maxValue_ = xmlStreamReader->readElementText().toInt();
			}
			else if(name == "Increment")
			{
				increment_ = xmlStreamReader->readElementText().toInt();
			}
			else if(name == "Units")
			{
				units_ = xmlStreamReader->readElementText();
			}
			else
			{
				//unexpected tag
				return false;
			}


		}
		xmlStreamReader->readNext();
	}


	return true;}

}; //namespace Picto
