#include <QtVariantProperty.h>
#include <QtPropertyBrowser.h>
#include "PropertyContainer.h"
#include "ColorProperty.h"
#include "PointProperty.h"
#include "RectProperty.h"
#include "../memleakdetect.h"

using namespace Picto;

PropertyContainer::PropertyContainer(QString _containerName)
:
containerName_(_containerName)
{
}

QSharedPointer<PropertyContainer> PropertyContainer::create(QString _containerName)
{
	QSharedPointer<PropertyContainer> returnVal(new PropertyContainer(_containerName));
	return returnVal;
}

void PropertyContainer::copyProperties(QSharedPointer<PropertyContainer> container2)
{
	QHash<QString, QVector<QSharedPointer<Property>>> cont2Props = container2->getProperties();
	QSharedPointer<Property> newProp;
	foreach(QVector<QSharedPointer<Property>> propVec,cont2Props)
	{
		foreach(QSharedPointer<Property> prop,propVec)
		{
			newProp = addProperty(prop->type(),prop->getName(),prop->value());
			QStringList attributes = prop->getAttributes();
			foreach(QString attribute,attributes)
			{
				newProp->setAttribute(attribute,prop->attributeValue(attribute));
			}
			newProp->setSelfPtr(newProp);
		}
	}
}

void PropertyContainer::setContainerName(QString _containerName)
{
	containerName_ = _containerName;
}

QString PropertyContainer::getContainerName()
{
	return containerName_;
}

int PropertyContainer::enumTypeId()
{
	return EnumProperty::typeId();
}

QSharedPointer<Property> PropertyContainer::addProperty(int _type, QString _identifier, QVariant _value, bool allowMultiple)
{
	Q_ASSERT_X(allowMultiple || !properties_.contains(_identifier),
		"PropertyContainer::addProperty",
		QString("Attempted to add multiple properties to a tag (%1) for which this operation is forbidden").arg(_identifier).toLatin1());

	QSharedPointer<Property> newProperty;
	if(_type == enumTypeId())
		newProperty = QSharedPointer<Property>( new EnumProperty(_identifier,_value) );
	else
	{
		switch(_type)
		{
		case QVariant::Rect:
			newProperty = QSharedPointer<Property>( new RectProperty(_identifier,_value) );
			break;
		case QVariant::Point:
			newProperty = QSharedPointer<Property>( new PointProperty(_identifier,_value) );
			break;
		case QVariant::Color:
			newProperty = QSharedPointer<Property>( new ColorProperty(_identifier,_value) );
			break;
		default:
			newProperty = QSharedPointer<Property>( new Property(_type,_identifier,_value) );
			break;
		}
	}
	connect(newProperty.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SIGNAL(propertyValueChanged(Property*,QVariant)));
	//containerGroupItem_->addSubProperty(newProperty);
	properties_[_identifier].push_back(newProperty);
	return newProperty;
}

QStringList PropertyContainer::getPropertyList()
{
	QStringList list;

	QHashIterator<QString, QVector<QSharedPointer<Property>>> propIterator(properties_);
	while(propIterator.hasNext())
	{
		propIterator.next();
		for(int i=0;i<propIterator.value().size();i++)
			list.push_back(propIterator.key());
	}
	return list;
}

QList<QSharedPointer<Property>> PropertyContainer::getRuntimeProperties()
{
	QList<QSharedPointer<Property>> runtimeProps;
	foreach(QVector<QSharedPointer<Property>> propVec,properties_)
	{
		foreach(QSharedPointer<Property> prop,propVec)
		{
			if(prop->isRuntimeEnabled())
				runtimeProps.push_back(prop);
		}
	}
	return runtimeProps;
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
	//containerGroupItem_.clear();
	//containerGroupItem_.clear();
	//propManager_->clear();

	////We just cleared out the group item.  Restore it.
	//QtVariantProperty *item = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(),
	//													  containerName_);
	//Q_ASSERT(item);
	//containerGroupItem_ = QSharedPointer<Property>( new Property(item,propManager_.data()) );

	//connect(propManager_.data(),
	//	    SIGNAL(valueChanged(QtProperty *, const QVariant &)),
	//	    this,
	//		SLOT(slotPropertyManagerValueChanged(QtProperty *, const QVariant &))
	//		);

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
		properties_[_propertyName][index]->setValue(_value);
	}
	return properties_[_propertyName][index];
}

//Sets all run values back to init values for all properties in this container
void PropertyContainer::setPropertiesToInitValues()
{
	foreach(QVector<QSharedPointer<Property>> propVec,properties_)
	{
		foreach(QSharedPointer<Property> prop,propVec)
		{
			prop->setValue(prop->initValue());
		}
	}
}

//QSharedPointer<Property> PropertyContainer::getPropertyFromQtProperty(QtProperty *property)
//{
//	QHashIterator<QString, QVector<QSharedPointer<Property>>> propIterator(properties_);
//	while(propIterator.hasNext())
//	{
//		propIterator.next();
//		int index = 0;
//		foreach(QSharedPointer<Property> prop,propIterator.value())
//		{
//			if(static_cast<QtProperty*>(prop->variantProp_) == property)
//			{
//				return prop;
//			}
//			index++;
//		}
//	}
//	return QSharedPointer<Property>();
//}

//void PropertyContainer::slotPropertyManagerValueChanged(QtProperty * property,
//														 const QVariant & value)
//{
//	QHashIterator<QString, QVector<QSharedPointer<Property>>> propIterator(properties_);
//	while(propIterator.hasNext())
//	{
//		propIterator.next();
//		int index = 0;
//		foreach(QSharedPointer<Property> prop,propIterator.value())
//		{
//			if(prop->variantProp_ == property)
//			{
//				emit signalPropertyValueChanged(propIterator.key(), index, value);
//				break;
//			}
//			index++;
//		}
//	}
//}

