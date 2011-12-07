//#include "PropertyLookup.h"
#include "../memleakdetect.h"
//
//namespace Picto {
//
//PropertyLookup::PropertyLookup()
//{
//}
//
//void PropertyLookup::addProperty(QSharedPointer<Property> prop)
//{
//	QString path = prop->getPath();
//	if(!prop->getParentAsset())
//		path.prepend("UIParameter::");
//	addProperty(prop->getIndex(),path);
//}
//
///*! \brief Turns the PropertyLookup into an XML fragment
// *
// *	The XML will look like this:
// *	<PropertyLookup time=123.4324 x=450 y=394/>
// */
//bool PropertyLookup::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("PropertyLookup");
//	foreach(PropInfo propInfo,lookupList_)
//	{
//		xmlStreamWriter->writeStartElement("Prop");
//		xmlStreamWriter->writeAttribute("id",QString("%1").arg(propInfo.id));
//		xmlStreamWriter->writeAttribute("path",propInfo.path);
//		xmlStreamWriter->writeEndElement();
//	}
//	xmlStreamWriter->writeEndElement();
//	return true;
//}
////! Converts XML into a PropertyLookup object.  Note that this deletes any existing data.
//bool PropertyLookup::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "PropertyLookup")
//	{
//		addError("PropertyLookup","Incorrect tag, expected <PropertyLookup>",xmlStreamReader);
//		return false;
//	}
//
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "PropertyLookup") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->name().toString();
//		if(name == "Prop")
//		{
//			int id;
//			QString path;
//			if(xmlStreamReader->attributes().hasAttribute("id"))
//			{
//				id = xmlStreamReader->attributes().value("id").toString().toInt();
//			}
//			else
//			{
//				addError("PropertyLookup","Data missing id attribute",xmlStreamReader);
//				return false;
//			}
//
//			if(xmlStreamReader->attributes().hasAttribute("path"))
//			{
//				path = xmlStreamReader->attributes().value("path").toString();
//			}
//			else
//			{
//				addError("PropertyLookup","Data missing path attribute",xmlStreamReader);
//				return false;
//			}
//
//			addProperty(id,path);
//		}
//		xmlStreamReader->readNext();
//	}
//	return true;
//}
//
//void PropertyLookup::addProperty(int id,QString path)
//{
//	PropInfo  propInf;
//	propInf.id = id;
//	propInf.path = path;
//	lookupList_.append(propInf);
//}
//
//void PropertyLookup::postDeserialize()
//{
//
//}
//
//bool PropertyLookup::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	return true;
//}
//
//
//}; //namespace Picto
