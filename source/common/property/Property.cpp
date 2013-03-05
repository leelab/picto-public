#include <QScriptValue>
#include <QObject>
#include <QMetaType>

#include "Property.h"
#include "../memleakdetect.h"

namespace Picto {

bool Property::hadObsoleteSerialSyntax_ = false;

Property::Property(int type, QString name, QVariant value) :
saveValue_(value),
initValue_(value),
runValue_(value),
type_(type),
name_(name),
tagName_(""),
typeVal_(""),
scriptEditable_(true),
runtimeEnabled_(false),
index_(-1),
assetId_(0),
visible_(true),
serialSyntaxUpgraded_(false),
syncInitAndRunVals_(false),
runMode_(false)
{
	//Add the ID serialization attribute so that we can read in this property's ID.
	AddSerializationAttribute("id");
}

Property::~Property()
{
}

int Property::type()
{
	return type_;
}

QString Property::getName()
{
	return name_;
}

void Property::setName(QString name)
{
	name_ = name;
}

QVariant Property::initValue()
{
	Q_ASSERT_X(runMode_,"Property::initValue","Init values should only be read in run mode");
	return initValuePrivate();
}

QVariant Property::value()
{
	if(runMode_)
	{
		return runValuePrivate();
	}
	else
		return saveValuePrivate();
}

void Property::enableRunMode(bool enable)
{
	runMode_ = enable;
}

void Property::enableInitRunValueSync(bool enable)
{
	syncInitAndRunVals_ = enable;
}


void Property::setAttribute(QString _attributeName, QVariant _attributeValue)
{
	attributes_[_attributeName] = _attributeValue;
}

QVariant Property::attributeValue(QString _attributeName) const
{
	if(attributes_.contains(_attributeName))
		return attributes_.value(_attributeName);
	return QVariant();
}

//void Property::addSubProperty(QSharedPointer<Property> prop)
//{
//	variantProp_->addSubProperty(prop->variantProp_);
//}

bool Property::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	if(!isNew() && !wasEdited() && !serialSyntaxUpgraded_)
	{
		QXmlStreamReader copyReader(tagText_);
		//Move to start element
		while(!copyReader.isStartElement())
			copyReader.readNext();
		while(!copyReader.atEnd())
		{
			xmlStreamWriter->writeCurrentToken(copyReader);
			copyReader.readNext();
		}
		return true;
	}
	if(tagName_ == "")
		tagName_ = getName();
	xmlStreamWriter->writeStartElement(tagName_);
	if(getName() != tagName_)
		xmlStreamWriter->writeAttribute("name",getName());
	// In cases where a Asset Factory used a type attribute to choose between types, a type that we don't use but need to write out would be in the tag.
	if(typeVal_ != "")
		xmlStreamWriter->writeAttribute("type",typeVal_);
	//UpdateSerializationAttributesFromValue();
	for(QMap<QString,QVariant>::iterator iter = serializationAttributes_.begin();iter != serializationAttributes_.end();iter++)
	{
		xmlStreamWriter->writeAttribute(iter.key(),iter.value().toString());
	}
	QString valToSave = variantToString(saveValuePrivate());
	if(valToSave.length())
		xmlStreamWriter->writeCharacters(valToSave);
	xmlStreamWriter->writeEndElement();
	return true;
}
bool Property::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader,bool)
{
	Q_ASSERT_X(!runMode_,"Property::deserializeFromXml","deserializeFromXml should not be called when property is in run mode.");
	serialSyntaxUpgraded_ = false;
	//Create XMLStreamWriter to store this DataStore's tag text in a string
	tagText_ = "";
	QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&tagText_));
	xmlWriter->writeCurrentToken(*xmlStreamReader);// Lets write the start tag to our tagText.

	//Get Start tag name (depending on who the parent is, they may have given us different names.
	tagName_ = xmlStreamReader->name().toString();
	bool emptyTag = true;
	bool hadName = false;
	if(xmlStreamReader->attributes().hasAttribute("name"))
	{
		setName(xmlStreamReader->attributes().value("name").toString());
		hadName = true;
	}
	else
		setName(tagName_);

	// In cases where a Asset Factory used a type attribute to choose between types, a type that we don't use but need to write out would be in the tag.
	if(xmlStreamReader->attributes().hasAttribute("type"))
	{
		typeVal_ = xmlStreamReader->attributes().value("type").toString();
	}
	else
		typeVal_ = "";

	//QXmlStreamAttributes attributeList = xmlStreamReader->attributes();
	//foreach(QXmlStreamAttribute attribute,attributeList)
	//{
	//	if(serializationAttributes_.contains(attribute.name().toString()))
	//	{
	//		serializationAttributes_[attribute.name().toString()] = attribute.value().toString();
	//		emptyTag = false;
	//	}
	//	else if((attribute.name() != "name") && (attribute.name() != "type") )
	//	{
	//		QString allowedAttributes = "name\ntype";
	//		foreach(QString attrName,serializationAttributes_.keys())
	//		{
	//			allowedAttributes.append("\n").append(attrName);
	//		}
	//		addError(getName().toLatin1(), QString("Invalid attribute: \n%1\nThe following attributes are defined for this property:\n%2\n-----").arg(attribute.name().toString()).arg(allowedAttributes).toLatin1(), xmlStreamReader);
	//		return false;
	//	}
	//}
	//Get the tag's attributes.  Put the attributes that are not expected (ie. obsolete) in an unexpectedAttributeMap
	QMap<QString,QVariant> unexpectedAttributeMap;
	QXmlStreamAttributes attributeList = xmlStreamReader->attributes();
	foreach(QXmlStreamAttribute attribute,attributeList)
	{
		if((attribute.name() != "name") && (attribute.name() != "type") )
		{
			emptyTag = false;
			if(serializationAttributes_.contains(attribute.name().toString()))
			{
				serializationAttributes_[attribute.name().toString()] = attribute.value().toString();
				continue;
			}
			//If we got here, there are unexpected attributes, tell the system that we encountered obsolete syntax.
			hadObsoleteSerialSyntax_ = true;
			serialSyntaxUpgraded_ = true;
			emit edited();
			unexpectedAttributeMap[attribute.name().toString()] = attribute.value().toString();
		}
	}

	//Loop until we're done with the tag or we reach the end of the XMLStream
	if(xmlStreamReader->readNext() == QXmlStreamReader::Invalid)
	{
		addError(getName().toLatin1(),QString("XML syntax has been violated.").toLatin1(),xmlStreamReader);
		return false;
	}
	xmlWriter->writeCurrentToken(*xmlStreamReader);// Write everything left to tagText.
	if(!xmlStreamReader->isCharacters() && !xmlStreamReader->isEndElement())
	{
		addError(getName().toLatin1(),QString("Unexpected value read.").toLatin1(),xmlStreamReader);
		return false;
	}
	QString value = xmlStreamReader->text().toString();

//	QString value = xmlStreamReader->readElementText();
	if(value != "")
		emptyTag = false;
	
	// If its an empty tag, we should just go with the default value.
	if(!emptyTag)
	{
		QString error = "";
		QVariant result = stringToVariant(value,error);
		if(!error.isEmpty())
		{	//Maybe this is an older version of a property that stored its data in attributes
			//Try to get a variant value from the attributes
			QString attributeError = "";
			result = attributeMapToVariantValue(unexpectedAttributeMap, attributeError);
			//If we couldn't get a value, report an error
			if(!attributeError.isEmpty())
				error = error + "\n" + attributeError;	//Add error details
			else
			{
				error = "";	//No more error
			}
		}
		if(error.size())
		{
			addError(getName().toLatin1(),error.toLatin1(),xmlStreamReader);
			return false;
		}
		
		//Check if our serialization syntax will be upgraded on the next save out.
		QString reserialized = variantToString(result);
		if(value != reserialized)
		{
			//If we got here, syntax will be changed on the next save, tell the system that we encountered obsolete syntax.
			hadObsoleteSerialSyntax_ = true;
			serialSyntaxUpgraded_ = true;
			emit edited();
		}
		setValue(result);
	}

	//Loop until we're done with the tag or we reach the end of the XMLStream
	while(!(xmlStreamReader->isEndElement() && (xmlStreamReader->name().toString() == tagName_)) && !xmlStreamReader->atEnd())
	{
		xmlStreamReader->readNext();
		xmlWriter->writeCurrentToken(*xmlStreamReader);// Write everything left to tagText.
	}
	//Make sure we didn't finish the document.
	if(xmlStreamReader->atEnd())
	{
		addError(getName().toLatin1(), "Unexpected end of document", xmlStreamReader);
		return false;
	}
	return true;
}

//This is the string that is sent over the network and written into the session database.
QString Property::initValToUserString()
{
	return variantToString(initValue());
}
//This updates the property init value from a string generated by toUserString().
void Property::initValFromUserString(QString userString)
{
	QString error = "";
	setInitValue(stringToVariant(userString,error));
}
//This is the string that is sent over the network and written into the session database.
QString Property::valToUserString()
{
	return variantToString(value());
}
//This updates the property value from a string generated by toUserString().
void Property::valFromUserString(QString userString)
{
	QString error = "";
	setValue(stringToVariant(userString,error));
}

int Property::getAssetId()
{
	return GetSerializationAttributeValue("id").toInt();
}
void Property::setAssetId(int id)
{
	SetSerializationAttributeValue("id",QVariant(id));
	//If the asset is new and was not edited, it doesn't need to
	//be serialized out because its a default property value, so 
	//we should not emit edited just because we are changing its
	//asset id.
	if(!isNew() || wasEdited())
		emit edited();
}

void Property::setInitValue(QVariant _value)
{
	Q_ASSERT_X(runMode_,"Property::setInitValue","Init values should only be set in run mode");
	setInitValuePrivate(_value);
	if(syncInitAndRunVals_)
	{
		setRunValuePrivate(_value);
	}
}

void Property::setValue(QVariant _value)
{
	if(!runMode_)
	{
		setSaveValuePrivate(_value);
		setInitValuePrivate(_value);
		setRunValuePrivate(_value);
	}
	else
	{
		setRunValuePrivate(_value);
		if(syncInitAndRunVals_)
			setInitValuePrivate(_value);
	}
}

QString Property::variantToString(QVariant value) const
{
	return value.toString();
}

QVariant Property::stringToVariant(QString string, QString& error) const
{
	error = "";
	return QVariant(string);
}

QVariant Property::attributeMapToVariantValue(QMap<QString,QVariant> attrMap, QString& error) const
{
	error = "This property type does not support value definition by attribute values.";
	return QVariant();
}

QVariant Property::saveValuePrivate()
{
	return saveValue_;
}
void Property::setSaveValuePrivate(QVariant _value)
{
	if(saveValue_ != _value)
	{
		saveValue_ = _value;
		unsavedValsInitialized_ = false;
		emit edited();
	}
}

QVariant Property::initValuePrivate()
{
	return initValue_;
}
void Property::setInitValuePrivate(QVariant _value)
{
	if(initValue_ != _value)
	{
		initValue_ = _value;
		emit initValueChanged(this,initValue_);
	}
}

QVariant Property::runValuePrivate()
{
	return runValue_;
}
void Property::setRunValuePrivate(QVariant _value)
{
	if(runValue_ != _value)
	{
		runValue_ = _value;
		emit valueChanged(this,runValue_);
	}
}

void Property::AddSerializationAttribute(QString name)
{
	SetSerializationAttributeValue(name,QVariant());
}

void Property::SetSerializationAttributeValue(QString name, QVariant _value)
{
	serializationAttributes_[name] = _value;
}

QVariant Property::GetSerializationAttributeValue(QString name)
{
	if(serializationAttributes_.contains(name))
		return serializationAttributes_[name];
	return QVariant();
}

}; //namespace Picto
