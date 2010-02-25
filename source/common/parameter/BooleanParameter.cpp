#include "BooleanParameter.h"

namespace Picto {

BooleanParameter::BooleanParameter()
{
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

/*! \brief Outputs a BooleanParameter to XML
 *
 *	The XML syntax is this:
 *		<Parameter operatorUI="true" type="Boolean">
 *	 		<Name>Joystick Visible During Padding</Name>
 *			<Order>3</Order>
 *			<Boolean default="true">
 *				<Label state="true">Visible</Label>
 *				<Label state="false">Invisible</Label>
 *			</Boolean>
 *		</Parameter>
 *
 */
bool BooleanParameter::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Parameter");
	if(bOperatorUI_)
		xmlStreamWriter->writeAttribute("operatorUI","true");
	else
		xmlStreamWriter->writeAttribute("operatorUI","false");
	xmlStreamWriter->writeAttribute("type", type_);

	xmlStreamWriter->writeTextElement("Name", name());
	xmlStreamWriter->writeTextElement("Order", QString("%1").arg(order_));

	xmlStreamWriter->writeStartElement("Boolean");
	if(defaultValue_)
		xmlStreamWriter->writeAttribute("default","true");
	else
		xmlStreamWriter->writeAttribute("default","false");

	xmlStreamWriter->writeStartElement("Label");
	xmlStreamWriter->writeAttribute("state","true");
	xmlStreamWriter->writeCharacters(trueLabel_);
	xmlStreamWriter->writeEndElement(); //Label

	xmlStreamWriter->writeStartElement("Label");
	xmlStreamWriter->writeAttribute("state","false");
	xmlStreamWriter->writeCharacters(falseLabel_);
	xmlStreamWriter->writeEndElement(); //Label

	xmlStreamWriter->writeEndElement(); //Boolean


	xmlStreamWriter->writeEndElement(); //Parameter
	return true;
}

 /*! \brief Modifies this BooleanParameter to match the passed in XML
 *
 *	The XML syntax is this:
 *		<Parameter operatorUI="true" type="Boolean">
 *	 		<Name>Joystick Visible During Padding</Name>
 *			<Order>3</Order>
 *			<Boolean default="true">
 *				<Label state="true">Visible</Label>
 *				<Label state="false">Invisible</Label>
 *			</Boolean>
 *		</Parameter>
 */

bool BooleanParameter::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
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
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Parameter") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//do nothing unless we're looking at a start element
		}
		else
		{
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
			else if(name == "Boolean")
			{
				QString defaultStr = xmlStreamReader->attributes().value("default").toString();
				if(defaultStr == "true")
					defaultValue_ = true;
				else if(defaultStr == "false")
					defaultValue_ = false;
				else 
					return false;
			}
			else if(name == "Label")
			{
				QString stateStr = xmlStreamReader->attributes().value("state").toString();
				if(stateStr == "true")
					trueLabel_ = xmlStreamReader->readElementText();
				else if(stateStr == "false")
					falseLabel_ = xmlStreamReader->readElementText();
				else
					return false;
			}
			else
			{
				//unexpected tag
				return false;
			}


		}
		xmlStreamReader->readNext();
	}


	return true;
}

}; //namespace Picto
