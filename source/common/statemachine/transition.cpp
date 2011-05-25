#include "Transition.h"

namespace Picto
{

Transition::Transition()
{
	AddDefinableProperty("Source","");
	AddDefinableProperty("SourceResult","");
	AddDefinableProperty("Destination","");
	source_ = "";
	sourceResult_ = "";
	destination_ = "";
}

Transition::Transition(QString source, QString sourceResult, QString destination)
{
	AddDefinableProperty("Source","");
	AddDefinableProperty("SourceResult","");
	AddDefinableProperty("Destination","");
	initializePropertiesToDefaults();
	propertyContainer_->setPropertyValue("Source",source);
	propertyContainer_->setPropertyValue("SourceResult",sourceResult);
	propertyContainer_->setPropertyValue("Destination",destination);
}

/*!	\brief Turns a transition into XML
 *
 *	The XML fragment will look like this
 *	<Transition>
 *		<Source>SourceElementName</Source>
 *		<SourceResult>SourceResultName</SourceResult>
 *		<Destination>SourceElementName</Destination>
 *	</Transition>
 */
//bool Transition::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("Transition");
//	xmlStreamWriter->writeTextElement("Source",source_);
//	xmlStreamWriter->writeTextElement("SourceResult",sourceResult_);
//	xmlStreamWriter->writeTextElement("Destination",destination_);
//	xmlStreamWriter->writeEndElement(); //Transition
//	return true;
//}
//
//bool Transition::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "Transition")
//	{
//		addError("Transition","Incorrect tag, expected <Transition>",xmlStreamReader);
//		return false;
//	}
//
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Transition") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->name().toString();
//		if(name == "Source")
//		{
//			source_ = xmlStreamReader->readElementText();
//		}
//		else if(name == "SourceResult")
//		{
//			sourceResult_ = xmlStreamReader->readElementText();
//		}
//		else if(name == "Destination")
//		{
//			destination_ = xmlStreamReader->readElementText();
//		}
//		else
//		{
//			addError("Transition", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//		xmlStreamReader->readNext();
//	}
//
//	if(xmlStreamReader->atEnd())
//	{
//		addError("Transition", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//
//	return true;
//}

bool Transition::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	source_ = propertyContainer_->getPropertyValue("Source").toString();
	sourceResult_ = propertyContainer_->getPropertyValue("SourceResult").toString();
	destination_ = propertyContainer_->getPropertyValue("Destination").toString();
	return true;
}

}