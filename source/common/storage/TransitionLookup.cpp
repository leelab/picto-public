#include "TransitionLookup.h"

namespace Picto {

TransitionLookup::TransitionLookup()
{
}

void TransitionLookup::addTransition(QSharedPointer<Transition> trans)
{
	addTransition(trans->getAssetId());
}

/*! \brief Turns the TransitionLookup into an XML fragment
 *
 *	The XML will look like this:
 *	<TransitionLookup time=123.4324 x=450 y=394/>
 */
bool TransitionLookup::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("TransitionLookup");
	foreach(int transInfo,lookupList_)
	{
		xmlStreamWriter->writeStartElement("Trans");
		xmlStreamWriter->writeAttribute("id",QString("%1").arg(transInfo));
		xmlStreamWriter->writeEndElement();
	}
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a TransitionLookup object.  Note that this deletes any existing data.
bool TransitionLookup::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "TransitionLookup")
	{
		addError("TransitionLookup","Incorrect tag, expected <TransitionLookup>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "TransitionLookup") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Trans")
		{
			int id;
			QString path,source,sourceResult,dest;
			if(xmlStreamReader->attributes().hasAttribute("id"))
			{
				id = xmlStreamReader->attributes().value("id").toString().toInt();
			}
			else
			{
				addError("TransitionLookup","Data missing id attribute",xmlStreamReader);
				return false;
			}

			addTransition(id);
		}
		xmlStreamReader->readNext();
	}
	return true;
}

void TransitionLookup::addTransition(int id)
{
	int  transInf = id;
	lookupList_.append(transInf);
}

void TransitionLookup::postDeserialize()
{

}

bool TransitionLookup::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
