#include "PropertyContainer.h"

#include <QtVariantProperty>

namespace Picto {

PropertyContainer::PropertyContainer(QString _containerName)
{
    containerGroupItem_ = variantManager_.addProperty(QtVariantPropertyManager::groupTypeId(),
													  _containerName);

	connect(&variantManager_,
		    SIGNAL(valueChanged(QtProperty *, const QVariant &)),
		    this,
			SLOT(slotPropertyManagerValueChanged(QtProperty *, const QVariant &))
			);
}

void PropertyContainer::setContainerName(QString _containerName)
{
	containerGroupItem_->setPropertyName(_containerName);
}

QString PropertyContainer::getContainerName()
{
	return containerGroupItem_->propertyName();
}

void PropertyContainer::addProperty(Property _property)
{
    QtVariantProperty *item = variantManager_.addProperty(_property.type(),
														  _property.name());

	//This will fail if you use an unsupported type
	Q_ASSERT(item);

	item->setValue(_property.value());

	QMap<QString, QVariant> attributes = _property.getAttributes();
	foreach(QString attributeName, attributes.keys())
	{
		item->setAttribute(attributeName, attributes.value(attributeName));
	}

	containerGroupItem_->addSubProperty(item);
	properties_[_property.name()] = item;
}

QList<QString> PropertyContainer::getPropertyList()
{
	return properties_.keys();
}

QVariant PropertyContainer::getPropertyValue(QString _propertyName)
{
	if(properties_.contains(_propertyName))
	{
		return variantManager_.value(properties_[_propertyName]);
	}
	else
	{
		return QVariant(QVariant::Invalid);
	}
}

void PropertyContainer::setPropertyValue(QString _propertyName, QVariant _value)
{
	if(properties_.contains(_propertyName))
	{
		properties_[_propertyName]->setValue(_value);
	}
	else
	{
		addProperty(Property(_value.type(), _propertyName, _value));
	}
}

void PropertyContainer::slotPropertyManagerValueChanged(QtProperty * property,
														 const QVariant & value)
{
	QMapIterator<QString, QtVariantProperty *> paramIterator(properties_);
	while(paramIterator.hasNext())
	{
		paramIterator.next();
		if(paramIterator.value() == property)
		{
			emit signalPropertyValueChanged(paramIterator.key(), value);
			break;
		}
	}
}

}; //namespace Picto
