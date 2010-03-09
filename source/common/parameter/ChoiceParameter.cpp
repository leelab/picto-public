#include "ChoiceParameter.h"

namespace Picto {

ChoiceParameter::ChoiceParameter()
{
	type_ = "Choice";
	currentOption_ = "";
	defaultOption_ = "";
}

Parameter* ChoiceParameter::NewParameter()
{
	return new ChoiceParameter;
}

/*!	\brief Adds a new choice to the end of the list */
bool ChoiceParameter::addChoice(QString label, QVariant data)
{
	//confirm that a choice with this label doesn't already exist
	if(options_.contains(label))
		return false;

	ChoiceParameterOption option;
	option.type = data.typeName();
	option.data = data;

	options_[label] = option;
	return true;
}

/*! \brief Removes a choice from the list.  Returns false if the passed in String isn't in the list */
bool ChoiceParameter::removeChoice(QString label)
{
	if(!options_.contains(label))
		return false;

	options_.remove(label);
	return true;
}

void ChoiceParameter::setDefaultOption(QString label)
{
	defaultOption_ = label;
}


/* \brief Sets the current value to the passed in string
 *
 *	Searches through the choices and finds the choice with a 
 *	matching label and makes that the current choice
 *	NOTE: This function is case sensitive!
 */
void ChoiceParameter::setValue(QVariant value)
{
	if(options_.contains(value.toString()))
		currentOption_ = value.toString();
}

//Returns the current value as a string
QVariant ChoiceParameter::getValue()
{
	if(options_.isEmpty() || !options_.contains(currentOption_))
		return QVariant("");
	else
		return options_[currentOption_].data;
}

/*!	\brief Equal to operation
 *
 *	Returns true if the passed in parameter's current choice data is equal
 *	to our current choice.  Note that this only compares the data, so there
 *	could be some weird issues...
 */
bool ChoiceParameter::equalTo(Parameter& RHS)
{
	return options_[currentOption_].data == RHS.getValue();
}

/*! \brief Equal to operation with a constant as RHS
 *
 *	There are a number of different types of constants that might be passed in
 *	and we can handle all of them.  If a string is passed in, we check to see 
 *	if it matches the label in the current choice.  If that fails (either because
 *	the passed in value isn't a string, or it doesn't match), we check to see if 
 *	it matches the type and value of the current data.
 */
bool ChoiceParameter::equalTo(QVariant& RHS)
{
	//check the passed in value to see if it matches our current label
	if(RHS.type() == QVariant::String &&
		RHS.toString() == currentOption_)
		return true;

	//check the passed in value to see if it matches our current data (in 
	//type and value)
	if(RHS.typeName() == options_[currentOption_].type)
	{
		if(options_[currentOption_].data == RHS)
			return true;
	}
	return false;
}

/*! \brief Outputs a ChoiceParameter to XML
 *
 *	The XML syntax is this:
 *
 *	<Parameter operatorUI="true" type="Choice">
 *		<Name>Target Color</Name>
 *		<Order>2</Order>
 *		<Choice default="Red">
 *			<Option>
 *				<Type>QColor</Type>
 *				<Label>Red</Label>
 *				<Data>
 *					<QColor R="255" G="0" B="0" A="0"/>
 *				</Data>
 *			</Option>
 *			<Option>
 *				<Type>QColor</Type>
 *				<Label>Green</Label>
 *				<Data>#00FF00</Data>
 *			</Option>
 *		</Choice>
 *	</Parameter>
 */
bool ChoiceParameter::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Parameter");
	if(bOperatorUI_)
		xmlStreamWriter->writeAttribute("operatorUI","true");
	else
		xmlStreamWriter->writeAttribute("operatorUI","false");
	xmlStreamWriter->writeAttribute("type", type_);

	xmlStreamWriter->writeTextElement("Name", name());
	xmlStreamWriter->writeTextElement("Order", QString("%1").arg(order_));

	xmlStreamWriter->writeStartElement("Choice");
	xmlStreamWriter->writeAttribute("default",defaultOption_);

	//print out the options
	QMap<QString, ChoiceParameterOption>::const_iterator option = options_.constBegin();
	while(option!= options_.end())
	{
		xmlStreamWriter->writeStartElement("Option");
		xmlStreamWriter->writeTextElement("Type",option.value().type);
		xmlStreamWriter->writeTextElement("Label",option.key());

		//if we're lucky, we can output the data as a string
		//this covers all numbers, dates, times, and strings
		xmlStreamWriter->writeStartElement("Data");
		if(!option.value().data.toString().isEmpty())
		{
			xmlStreamWriter->writeCharacters(option.value().data.toString());
		}
		else
		{
			//we're dealing with an unrecognized data type...
			return false;
		}
		xmlStreamWriter->writeEndElement(); //Data

		xmlStreamWriter->writeEndElement(); //Option

		option++;
	}

	xmlStreamWriter->writeEndElement(); //Choice
	xmlStreamWriter->writeEndElement(); //Parameter

	return true;
}

/*! \brief Modifies this ChoiceParameter to match XML
 *
 *	The XML syntax is this:
 *
 *	<Parameter operatorUI="true" type="Choice">
 *		<Name>Target Color</Name>
 *		<Order>2</Order>
 *		<Choice default="Red">
 *			<Option>
 *				<Type>QColor</Type>
 *				<Label>Red</Label>
 *				<Data>
 *					<QColor R="255" G="0" B="0" A="0"/>
 *				</Data>
 *			</Option>
 *			<Option>
 *				<Type>QColor</Type>
 *				<Label>Green</Label>
 *				<Data>
 *					<QColor R="255" G="0" B="0" A="0"/>
 *				</Data>
 *			</Option>
 *		</Choice>
 *	</Parameter>
 */
bool ChoiceParameter::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "Parameter")
	{
		addError("ChoiceParameter","Incorrect tag, expected <Parameter>",xmlStreamReader);
		return false;
	}
	if(xmlStreamReader->attributes().value("type").toString() != type_)
	{
		addError("ChoiceParameter","Incorrect type of parameter",xmlStreamReader);
		return false;
	}

	//We should clear out the option list in cse this isn't a 
	//fresh instance
	options_.clear();

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
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}
		QString name = xmlStreamReader->name().toString();
		if(name == "Name")
		{
			name_ = xmlStreamReader->readElementText();
		}
		else if(name == "Order")
		{
			order_ = xmlStreamReader->readElementText().toInt();
		}
		else if(name == "Choice")
		{
			defaultOption_ = xmlStreamReader->attributes().value("default").toString();
		}
		else if(name == "Option")
		{
			ChoiceParameterOption newOption;
			QString newOptionLabel;

			xmlStreamReader->readNext();
			while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Option"))
			{
				if(!xmlStreamReader->isStartElement())
				{
					//Do nothing unless we're at a start element
					xmlStreamReader->readNext();
					continue;
				}
				QString name = xmlStreamReader->name().toString();
				if(name == "Type")
				{
					newOption.type = xmlStreamReader->readElementText();
				}
				else if(name == "Label")
				{
					newOptionLabel = xmlStreamReader->readElementText();
				}
				else if(name == "Data")
				{
					QString elementText = xmlStreamReader->readElementText();
					if(!elementText.isEmpty())
					{
						newOption.data = elementText;
						newOption.data.convert(QVariant::nameToType(newOption.type.toAscii()));
					}
					else
					{
						addError("ChoiceParameter", "Unsupported data type in option", xmlStreamReader);
						return false;
					}
				}
				else
				{
					addError("ChoiceParameter", "Unexpected tag within <Option>", xmlStreamReader);
					return false;
				}
				xmlStreamReader->readNext();
			}
			options_[newOptionLabel] = newOption;

		}
		else
		{
			addError("ChoiceParameter", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("ChoiceParameter", "Unexpected end of document", xmlStreamReader);
		return false;
	}
	return true;
}

}; //namespace Picto
