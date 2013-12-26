#include <QtVariantProperty.h>
#include <QtPropertyBrowser.h>
#include "PropertyContainer.h"
#include "ColorProperty.h"
#include "PointProperty.h"
#include "SizeProperty.h"
#include "ListProperty.h"
#include "MapProperty.h"
#include "../memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new PropertyContainer with the input name.
 *	\note The _containerName is not currently used in Picto, nor is there any foreseeable reason that it would be used.
 *	We should probably get rid of it.
 */
PropertyContainer::PropertyContainer(QString _containerName)
:
containerName_(_containerName)
{
}

/*! \brief Creates a new PropertyContainer with the input name and returns a shared pointer to it.
 *	\sa PropertyContainer::PropertyContainer()
 */
QSharedPointer<PropertyContainer> PropertyContainer::create(QString _containerName)
{
	QSharedPointer<PropertyContainer> returnVal(new PropertyContainer(_containerName));
	return returnVal;
}

/*! \brief Copies all Properties from the input PropertyContainer to this one.
 *	\details This is a "deep-copy."  New Property objects are created and initialized
 *	with the data of the input PropertyContainer's Properties.  We are not copying
 *	pointers.
 */
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

/*! \brief Sets this container's name.
 *	\note The _containerName is not currently used in Picto, nor is there any foreseeable reason that it would be used.
 *	We should probably get rid of it.
 */
void PropertyContainer::setContainerName(QString _containerName)
{
	containerName_ = _containerName;
}

/*! \brief Gets this container's name.
 *	\note The containerName is not currently used in Picto, nor is there any foreseeable reason that it would be used.
 *	We should probably get rid of it.
 */
QString PropertyContainer::getContainerName()
{
	return containerName_;
}

/*! \brief Returns the type Id to be used when creating EnumProperty objects with addProperty().
 *	\details This is needed specifically for the EnumProperty since the other Property objects Ids are
 *	based on the QVariant::type values.
 */
int PropertyContainer::enumTypeId()
{
	return EnumProperty::typeId();
}

/*! \brief Adds a new Property to this PropertyContainer of the input _type, with the name: _identifier and the input _value, then returns it.
 *	\details allowMultiple was once used to allow for more than one Property with the same _identifier; however, this doesn't
 *	really make sense anymore with the current design.  I believer that it is no longer used and we should probably remove it.
 */
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
		case QVariant::Size:
			newProperty = QSharedPointer<Property>( new SizeProperty(_identifier,_value) );
			break;
		case QVariant::Point:
			newProperty = QSharedPointer<Property>( new PointProperty(_identifier,_value) );
			break;
		case QVariant::Color:
			newProperty = QSharedPointer<Property>( new ColorProperty(_identifier,_value) );
			break;
		case QVariant::List:
			newProperty = QSharedPointer<Property>( new ListProperty(_identifier,_value) );
			break;
		case QVariant::Map:
			newProperty = QSharedPointer<Property>( new MapProperty(_identifier,_value) );
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

/*! \brief Returns a list of the names of the Properties in this PropertyContainer.
*/
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

/*! \brief Returns a list of the Property objects that have been set as RuntimeProperties (ie. Their initValues will be settable during runtime)
 *	\sa Property::setRuntimeEditable()
 */
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

/*! \brief Returns a pointer to the Property with the input _identifier name.
 *	\details index was used when more than one Property with the same name was allowed.  This is not currently used though
 *	and so it should really just always be 0.  We should remove this option at some point.
 *	\sa addProperty()
 */
QSharedPointer<Property> PropertyContainer::getProperty(QString _identifier,int index)
{
	if(!properties_.contains(_identifier))
		return QSharedPointer<Property>();
	if(properties_[_identifier].size() <= index)
		return QSharedPointer<Property>();
	return properties_[_identifier][index];
}

/*! \brief Sets all contained Property objects as Associate Properties for the purpose of runtime checking for Analysis Scripts
 *	setting Experimental Property values.
 *	\sa Property::setAssociateProperty()
 */
void PropertyContainer::setPropertiesAsAssociates(bool toAssociate)
{
	foreach(QVector<QSharedPointer<Property>> propVec,properties_)
	{
		foreach(QSharedPointer<Property> prop,propVec)
		{
			prop->setAssociateProperty(toAssociate);
		}
	}
}

/*! \brief Clears all the Property objects out of this container.
 *	\note I don't think that this is actually used in Picto.  We should check and if not get rid of it.
*/
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

/*! \brief Returns the value of the Property with the input _identifier name.
 *	\details index was used when more than one Property with the same name was allowed.  This is not currently used though
 *	and so it should really just always be 0.  We should remove this option at some point.
 *	\sa getProperty(), addProperty()
 */
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

/*! \brief This is left over from an old version of this class when a Property identifier and name weren't necessarily the same thing.
 *	It doesn't appear to be used and should probably be removed.
 */
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

/*! \brief Sets the value of the Property with the input _propertyName to the input _value.
 *	\details This is a convenience function mostly equivalent to \code getProperty(_propertyName)->setValue(_value) \endcode
 *	It does add some functionality though.  If no Property exists with the input _propertyName, it is created automatically
 *	according to the type of the _value input.  Since Property objects are really only created from PropertyFactory objects 
 *	at this point though, the auto-addProperty() functionality should really never be used.  It would probably be a good 
 *	idea to look into this and get rid of it.
 *	\note index was used when more than one Property with the same name was allowed.  This is not currently used though
 *	and so it should really just always be 0.  We should remove this option at some point.
 *	\sa getProperty(), addProperty()
 */
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

/*! \brief Sets the runValues of all contained Property objects to their initValues.
 *	\details This is useful for reseting a DataStore's Property values when it enters scope.
 */
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

