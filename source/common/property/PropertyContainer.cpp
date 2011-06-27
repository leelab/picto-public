#include "PropertyContainer.h"
#include <QtVariantProperty>
#include <QtProperty>
#include "EnumProperty.h"
#include "ColorProperty.h"
#include "PointProperty.h"
#include "RectProperty.h"

using namespace Picto;

PropertyContainer::PropertyContainer(QString _containerName)
:
containerName_(_containerName)
{
	clear();
}

//We need to do some pointer gymnastics to get around the fact that
//the QtAbstractProperyManager deletes all of its properties when it
//is deleted but we would like to have shared pointers for the
//properties such that they aren't deleted until we're totally done 
//with them.  To do this, we only allow the PropertyContainer (inherits
//the QtPropertyManager) to be created with a shared pointer.  Whenever
//a property is added to it, this shared pointer is added to the property.
//This means that the ProperyContainer will never be deleted until all of
//its properties have already been deleted.  Whenever the properties are 
//deleted, they remove themselves from their parent PropertyManager's list.  
//This way, when the PropertyManager is deleted, all of its children will 
//have already been removed from its list and there will be no problem.
//Tada!
QSharedPointer<PropertyContainer> PropertyContainer::create(QString _containerName)
{
	QSharedPointer<PropertyContainer> returnVal(new PropertyContainer(_containerName));
	return returnVal;
}

void PropertyContainer::setContainerName(QString _containerName)
{
	containerGroupItem_->setName(_containerName);
}

QString PropertyContainer::getContainerName()
{
	return containerGroupItem_->getName();
}

QSharedPointer<Property> PropertyContainer::addProperty(int _type, QString _identifier, QVariant _value, bool allowMultiple)
{
	Q_ASSERT_X(allowMultiple || !properties_.contains(_identifier),
		"PropertyContainer::addProperty",
		QString("Attempted to add multiple properties to a tag (%1) for which this operation is forbidden").arg(_identifier).toAscii());

	QtVariantProperty *item = propManager_->addProperty(_type,
														  _identifier);
	//This will fail if you use an unsupported type
	Q_ASSERT(item);

	item->setValue(_value);

	QSharedPointer<Property> newProperty;
	if(_type == QtVariantPropertyManager::enumTypeId())
		newProperty = QSharedPointer<Property>( new EnumProperty(QSharedPointer<QtVariantProperty>(item),propManager_) );
	else
	{
		switch(_type)
		{
		case QVariant::Rect:
			newProperty = QSharedPointer<Property>( new RectProperty(QSharedPointer<QtVariantProperty>(item),propManager_) );
			break;
		case QVariant::Point:
			newProperty = QSharedPointer<Property>( new PointProperty(QSharedPointer<QtVariantProperty>(item),propManager_) );
			break;
		case QVariant::Color:
			newProperty = QSharedPointer<Property>( new ColorProperty(QSharedPointer<QtVariantProperty>(item),propManager_) );
			break;
		default:
			newProperty = QSharedPointer<Property>( new Property(QSharedPointer<QtVariantProperty>(item),propManager_) );
			break;
		}
	}
	containerGroupItem_->addSubProperty(newProperty);
	properties_[_identifier].push_back(newProperty);
	return newProperty;
}

QStringList PropertyContainer::getPropertyList()
{
	QStringList list;

	QMapIterator<QString, QVector<QSharedPointer<Property>>> propIterator(properties_);
	while(propIterator.hasNext())
	{
		propIterator.next();
		for(int i=0;i<propIterator.value().size();i++)
			list.push_back(propIterator.key());
	}
	return list;
}

QSharedPointer<Property> PropertyContainer::getProperty(QString _identifier,int index)
{
	if(!properties_.contains(_identifier))
		return QSharedPointer<Property>();
	if(properties_[_identifier].size() <= index)
		return QSharedPointer<Property>();
	return properties_[_identifier][index];
}

void PropertyContainer::clear()
{

	properties_.clear();
	containerGroupItem_.clear();
	//We can't just clear the propManager_ because we need its properties to clear themselves out of it
	//before its deleted (see comment in Property destructor).  Instead, we just replace the propManager_
	//with a new one, and let the old one get deleted when its pointers go out of scope.
	propManager_ = QSharedPointer<QtVariantPropertyManager>(new QtVariantPropertyManager());
	QtVariantProperty *item = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(),
														  containerName_);
	Q_ASSERT(item);
	containerGroupItem_ = QSharedPointer<Property>( new Property(QSharedPointer<QtVariantProperty>(item),propManager_) );

	connect(propManager_.data(),
		    SIGNAL(valueChanged(QtProperty *, const QVariant &)),
		    this,
			SLOT(slotPropertyManagerValueChanged(QtProperty *, const QVariant &))
			);

}

QVariant PropertyContainer::getPropertyValue(QString _identifier, int index)
{
	if(properties_.contains(_identifier) && (properties_[_identifier].size() > index))
	{
		return properties_.value(_identifier)[index]->value();
	}
	else
	{
		return QVariant(QVariant::Invalid);
	}
}

QString PropertyContainer::getPropertyName(QString _identifier, int index)
{
	if(properties_.contains(_identifier) && (properties_[_identifier].size() > index))
	{
		return properties_.value(_identifier)[index]->getName();
	}
	else
	{
		return QString("");
	}
}

QSharedPointer<Property> PropertyContainer::setPropertyValue(QString _propertyName, QVariant _value, int index)
{
	if(properties_.contains(_propertyName) && (properties_[_propertyName].size() > index))
	{
		properties_[_propertyName][index]->setValue(_value);
	}
	else
	{
		addProperty(_value.type(), _propertyName, _value);
		index = properties_[_propertyName].size()-1;
	}
	return properties_[_propertyName][index];
}

QSharedPointer<Property> PropertyContainer::getPropertyFromQtProperty(QtProperty *property)
{
	QMapIterator<QString, QVector<QSharedPointer<Property>>> propIterator(properties_);
	while(propIterator.hasNext())
	{
		propIterator.next();
		int index = 0;
		foreach(QSharedPointer<Property> prop,propIterator.value())
		{
			if(prop->variantProp_.staticCast<QtProperty>().data() == property)
			{
				return prop;
			}
			index++;
		}
	}
	return QSharedPointer<Property>();
}

void PropertyContainer::slotPropertyManagerValueChanged(QtProperty * property,
														 const QVariant & value)
{
	QMapIterator<QString, QVector<QSharedPointer<Property>>> propIterator(properties_);
	while(propIterator.hasNext())
	{
		propIterator.next();
		int index = 0;
		foreach(QSharedPointer<Property> prop,propIterator.value())
		{
			if(prop->variantProp_.data() == property)
			{
				emit signalPropertyValueChanged(propIterator.key(), index, value);
				break;
			}
			index++;
		}
	}
}

