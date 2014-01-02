#include "ObsoleteNameAsset.h"
#include "../memleakdetect.h"

namespace Picto {

ObsoleteNameAsset::ObsoleteNameAsset()
{
	assetId_ = 0;
}

ObsoleteNameAsset::~ObsoleteNameAsset()
{
}

/*! \brief Constructs and returns a shared pointer to a new ObsoleteNameAsset.*/
QSharedPointer<Asset> ObsoleteNameAsset::Create()
{
	return QSharedPointer<Asset>(new ObsoleteNameAsset());
}

/*! \brief Implements Asset::serializeAsXml() to serialize this ObsoleteNameAsset back out exactly as it was serialized in but with the new tag name set in setNewTagName().*/
bool ObsoleteNameAsset::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	Q_ASSERT_X(tagText_ != "","DataStore::serializeAsXml","This datastore has no tag text, it was either not serialized in or not initialized to default values.");
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(tagText_));

	//Move read pointer to the first token
	while(!xmlReader->isStartElement())
		xmlReader->readNext();

	//Get the original tag name
	QString origTagName = xmlReader->name().toString();
	//Write out first token with new name.
	xmlStreamWriter->writeStartElement(newTagName_);
	foreach(QXmlStreamAttribute attribute,xmlReader->attributes())
	{
		xmlStreamWriter->writeAttribute(attribute.name().toString(),attribute.value().toString());
	}

	//Write out all contents of top level tag
	int descendantLevel = 0;
	while(xmlReader->readNext() && !(xmlReader->isEndElement() && (xmlReader->name().toString() == origTagName) && !descendantLevel) && !xmlReader->atEnd())
	{
		xmlStreamWriter->writeCurrentToken(*xmlReader);// Serialize out whatever we read
		//Continue loop if we're not at a start element
		if(xmlReader->isStartElement())
		{
			descendantLevel++;
		}
		else if(xmlReader->isEndElement())
			descendantLevel--;
	}
	//write out end tag
	xmlStreamWriter->writeEndElement();
	return true;
}
/*! \brief Implements Asset::deserializeFromXml() to deserialize this ObsoleteNameAsset so that it can have its tag name changed and be serialized back out.*/
bool ObsoleteNameAsset::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Create XMLStreamWriter to store this DataStore's tag text in a string
	tagText_ = "";
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&tagText_));
	xmlWriter->writeCurrentToken(*xmlStreamReader);// Lets write the start tag to our tagText.

	//Initilialize this DataStore's deserialization
	bool returnVal = true;
	//Get Start tag name (depending on who the parent is, they may have given us different names.
	QString myTagName = xmlStreamReader->name().toString();
	newTagName_ = myTagName;
	//Make sure its a start tag
 	if(!xmlStreamReader->isStartElement())
	{
		addError(QString("Expected start element").toLatin1());
		return false;
	}

	//Get attribute data
	foreach(QXmlStreamAttribute attribute,xmlStreamReader->attributes())
	{
		if(attribute.name() == "id")	//Get the AssetId
		{
			assetId_ = attribute.value().toString().toInt();
			break;
		}
		else if(attribute.name() == "type")	//Get the type name
		{
			origType_ = attribute.value().toString();
		}
	}

	//Loop until we're done with the tag or we reach the end of the XMLStream
	int descendantLevel = 1;	//Don't stop until we're at level zero (there might be descendants with our same tag name so we need to track this
	while(!(xmlStreamReader->isEndElement() && (xmlStreamReader->name().toString() == myTagName) && !descendantLevel) && !xmlStreamReader->atEnd())
	{
		//Move forward in XMLStream
		if(xmlStreamReader->readNext() == QXmlStreamReader::Invalid)
		{
			addError(QString("XML syntax has been violated.").toLatin1());
			return false;
		}
		xmlWriter->writeCurrentToken(*xmlStreamReader);// Lets add the current tag to our tagText.
		//If we're at an end element, decrement the desendant level and continue loop
		if(xmlStreamReader->isEndElement())
		{
			descendantLevel--;
			continue;
		}
		//Continue loop if we're not at a start element
		if(!xmlStreamReader->isStartElement())
			continue;
		descendantLevel++;
		//continue the loop
	}
	//Make sure we didn't finish document
	if(xmlStreamReader->atEnd())
	{
		returnVal = false;
		addError("Unexpected end of document");
	}
	return returnVal;
}

/*! \brief Extends DataStore::postDeserialize() to emit the edited() signal and tell this objects parent that it changed.*/
void ObsoleteNameAsset::postDeserialize()
{
	DataStore::postDeserialize();
	reportOldVersionAsset();
	emit edited();
}

/*! \brief Sets the new name that should be used in this object's serialized XML tag.*/
void ObsoleteNameAsset::setNewTagName(QString newName)
{
	newTagName_ = newName;
}

/*! \brief Returns the value of the "type" attribute in the XML that was deserialized into this object.*/
QString ObsoleteNameAsset::getOriginalType()
{
	return origType_;
}

}; //namespace Picto
