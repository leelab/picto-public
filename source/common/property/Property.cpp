#include <QScriptValue>
#include <QObject>

#include "Property.h"

namespace Picto {

Property::Property(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager) :
variantProp_(variantProp),
manager_(manager),
tagName_("")
{
	connect(manager_.data(),SIGNAL(valueChanged()),this,SLOT(valueChanged()));
	connect(manager_.data(),SIGNAL(attributeChanged()),this,SLOT(attributeChanged()));
}

Property::~Property()
{
	//We don't want the QtVariantPropertyManager to destory properties
	//because we would like them to be accessible until know one needs them
	//anymore.  For this reason, we store a shared pointer to the manager in
	//this class.  That way, it can't be destroyed until all of its properties
	//are destroyed.  It is important, however, that this objects contained 
	//QtVariantProperty be destroy before the manager because when it does that
	//it removes itself from the manager, thereby keeping the manager and the 
	//manager doesn't try to delete it.  Otherwise, the manager would delete it
	//and then we would try to delete it again when our shared pointer goes out 
	//of scope.
	variantProp_.clear();
}

int Property::type()
{
	return variantProp_->valueType();
}

QString Property::name()
{
	return variantProp_->propertyName();
}

void Property::setName(QString name)
{
	variantProp_->setPropertyName(name);
}

QVariant Property::value()
{
	return variantProp_->value();
}

QString Property::valueString()
{
	return variantProp_->value().toString();
}

void Property::setValue(QVariant _value)
{
	variantProp_->setValue(_value);
	//! \TODO:  Verify that edited is emited when setValue is called.
}

void Property::addAttribute(QString _attributeName, QVariant _attributeValue)
{
	variantProp_->setAttribute(_attributeName,_attributeValue);
}

void Property::addSubProperty(QSharedPointer<Property> prop)
{
	variantProp_->addSubProperty(prop->variantProp_.data());
}

bool Property::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	if(tagName_ == "")
		tagName_ = name();
	xmlStreamWriter->writeStartElement(tagName_);
	if(name() != tagName_)
		xmlStreamWriter->writeAttribute("name",name());
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
bool Property::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Get Start tag name (depending on who the parent is, they may have given us different names.
	tagName_ = xmlStreamReader->name().toString();
	if(xmlStreamReader->attributes().hasAttribute("name"))
		setName(xmlStreamReader->attributes().value("name").toString());
	else
		setName(tagName_);
	for(QMap<QString,QVariant>::iterator iter = serializationAttributes_.begin();iter != serializationAttributes_.end();iter++)
	{
		if(xmlStreamReader->attributes().hasAttribute(iter.key()))
			iter.value() = xmlStreamReader->attributes().value(iter.key()).toString();
	}
	QString value = xmlStreamReader->readElementText();
	SetValueFromDeserializedData(value);

	//Loop until we're done with the tag or we reach the end of the XMLStream
	while(!(xmlStreamReader->isEndElement() && (xmlStreamReader->name().toString() == tagName_)) && !xmlStreamReader->atEnd())
		xmlStreamReader->readNext();
	//Make sure we didn't finish the document.
	if(xmlStreamReader->atEnd())
	{
		addError(tagName_.toAscii(), "Unexpected end of document", xmlStreamReader);
		return false;
	}
	return true;
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

void Property::SetValueFromDeserializedData(QVariant _value)
{
	setValue(_value);
}

void Property::valueChanged(QtProperty *property, const QVariant &val)
{
	if(property == variantProp_.data())
		emit edited();
}
void Property::attributeChanged(QtProperty *property,
            const QString &attribute, const QVariant &val)
{
	if(property == variantProp_.data())
		emit edited();
}

}; //namespace Picto
