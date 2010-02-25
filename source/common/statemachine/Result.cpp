#include "Result.h"

namespace Picto
{

Result::Result()
{
	type_ = "Result";
}

QString Result::run()
{
	return resultValue_;
}


/*!	\Brief Turns a result object into an XML framgent.
 *
 *	The XML fragment for a result will look like this:
 * 
 *	<Result>
 *		<ID>{d2c2b990-2227-11df-8a39-0800200c9a66}</ID>
 *		<Name>BrokeFixation</Name>
 *	</Result>
 */
bool Result::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Result");
	
	xmlStreamWriter->writeTextElement("ID",getId().toString());
	xmlStreamWriter->writeTextElement("Name",name_);

	xmlStreamWriter->writeEndElement(); //Result
	return true;
}


bool Result::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement())
		return false;
	if(xmlStreamReader->name() != "Result")
		return false;

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Result") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "ID")
		{
			QString idStr = xmlStreamReader->readElementText();
			setId(QUuid(idStr));
			registerObject();
		}
		else if(name == "Name")
		{
			name_ = xmlStreamReader->readElementText();
		}
		else
		{
			return false;
		}
		xmlStreamReader->readNext();
	}
	
	if(xmlStreamReader->atEnd())
		return false;

	return true;
}
}//namespace Picto