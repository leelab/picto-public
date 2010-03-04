#include "Result.h"

namespace Picto
{

Result::Result()
{
	propertyContainer_.setPropertyValue("Type","Result");
}

QString Result::run()
{
	return propertyContainer_.getPropertyValue("Name").toString();
}


/*!	\Brief Turns a result object into an XML framgent.
 *
 *	The XML fragment for a result will look like this:
 * 
 *	<StateMachineElement type="Result">
 *		<Name>BrokeFixation</Name>
 *	</StateMachineElement>
 */
bool Result::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("StateMachineElement");
	xmlStreamWriter->writeAttribute("type","Result");
	
	xmlStreamWriter->writeTextElement("Name", propertyContainer_.getPropertyValue("Name").toString());

	xmlStreamWriter->writeEndElement(); //StateMachineElement
	return true;
}


bool Result::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateMachineElement")
	{
		addError("Result","Incorrect tag, expected <StateMachineElement>",xmlStreamReader);
		return false;
	}
	if(xmlStreamReader->attributes().value("type").toString() != type())
	{
		addError("Result","Incorrect type of StateMachineElement, expected Result",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateMachineElement") && !xmlStreamReader->atEnd())
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
			propertyContainer_.setPropertyValue("Name",QVariant(xmlStreamReader->readElementText()));
		}
		else
		{
			addError("Result", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}
	
	if(xmlStreamReader->atEnd())
	{
		addError("Result", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	return true;
}
}//namespace Picto