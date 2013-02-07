#include <QScriptValue>
#include <QObject>

#include "Property.h"
#include "../memleakdetect.h"

namespace Picto {

Property::Property(int type, QString name, QVariant value) :
value_(value),
type_(type),
name_(name),
tagName_(""),
typeVal_(""),
scriptEditable_(true),
runtimeEnabled_(false),
index_(-1),
assetId_(0)
{
	//Add the ID serialization attribute so that we can read in this property's ID.
	AddSerializationAttribute("id");

	//connect(manager,SIGNAL(valueChanged(QtProperty *, const QVariant &)),this,SLOT(valueChanged(QtProperty *, const QVariant &)));
	//connect(manager,SIGNAL(attributeChanged(QtProperty*,const QString&, const QVariant&)),this,SLOT(attributeChanged(QtProperty*,const QString&, const QVariant&)));
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

QVariant Property::value()
{
	return value_;
}

QString Property::valueString()
{
	return value().toString();
}

void Property::setValue(QVariant _value)
{
	if(value_ != _value)
	{
		value_ = _value;
		emit edited();
		emit valueChanged(selfPtr().staticCast<Property>());
	}
}

void Property::setAttribute(QString _attributeName, QVariant _attributeValue)
{
	attributes_[_attributeName] = _attributeValue;
}

QVariant Property::attributeValue(QString _attributeName)
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
	if(!isNew() && !wasEdited())
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
	UpdateSerializationAttributesFromValue();
	for(QMap<QString,QVariant>::iterator iter = serializationAttributes_.begin();iter != serializationAttributes_.end();iter++)
	{
		xmlStreamWriter->writeAttribute(iter.key(),iter.value().toString());
	}
	QString value = valueString();
	if(value.length())
		xmlStreamWriter->writeCharacters(valueString());
	xmlStreamWriter->writeEndElement();
	return true;
}
bool Property::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader,bool)
{
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

	QXmlStreamAttributes attributeList = xmlStreamReader->attributes();
	foreach(QXmlStreamAttribute attribute,attributeList)
	{
		if(serializationAttributes_.contains(attribute.name().toString()))
		{
			serializationAttributes_[attribute.name().toString()] = attribute.value().toString();
			emptyTag = false;
		}
		else if((attribute.name() != "name") && (attribute.name() != "type") )
		{
			QString allowedAttributes = "name\ntype";
			foreach(QString attrName,serializationAttributes_.keys())
			{
				allowedAttributes.append("\n").append(attrName);
			}
			addError(getName().toLatin1(), QString("Invalid attribute: \n%1\nThe following attributes are defined for this property:\n%2\n-----").arg(attribute.name().toString()).arg(allowedAttributes).toLatin1(), xmlStreamReader);
			return false;
		}
	}
	//for(QMap<QString,QVariant>::iterator iter = serializationAttributes_.begin();iter != serializationAttributes_.end();iter++)
	//{
	//	if(xmlStreamReader->attributes().hasAttribute(iter.key()))
	//	{
	//		iter.value() = xmlStreamReader->attributes().value(iter.key()).toString();
	//		emptyTag = false;
	//	}
	//}


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
		if(!SetValueFromString(value,xmlStreamReader))
			return false;
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
QString Property::toUserString()
{
	return valueString();
}
//This updates the property value from a string generated by toUserString().
void Property::fromUserString(QString userString)
{
	setValue(userString);
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

void Property::setValueFromProp(QSharedPointer<Property> prop)
{
	setValue(prop->value());
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

void Property::UpdateSerializationAttributesFromValue()
{
}

bool Property::SetValueFromString(QVariant _value, QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	setValue(_value);
	return true;
}

//void Property::valueChanged(QtProperty *property, const QVariant &)
//{
//	if(property == variantProp_)
//	{
//		emit edited();
//		emit valueChanged(selfPtr().staticCast<Property>());
//	}
//}
//void Property::attributeChanged(QtProperty *property,
//            const QString &, const QVariant &)
//{
//	if(property == variantProp_)
//	{
//		emit edited();
//		emit valueChanged(selfPtr().staticCast<Property>());
//	}
//}

}; //namespace Picto
