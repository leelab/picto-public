#include "PropertyManager.h"
#include "../operator/DataViewSpecs.h"

#include "../../common/memleakdetect.h"
using namespace Picto;


PropertyManager::PropertyManager(QWidget *parent)
	: QtVariantPropertyManager(parent), creatingType(0)
{
}

int PropertyManager::viewPropertyTypeId()
{
	return qMetaTypeId<ViewProperties>();
}

bool PropertyManager::isPropertyTypeSupported(int propertyType) const
{
	if (propertyType == viewPropertyTypeId())
		return true;
	return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}


int PropertyManager::valueType(int propertyType) const
{
	if (propertyType == viewPropertyTypeId())
	{
		return QVariant::fromValue(ViewProperties()).userType();
	}
	return QtVariantPropertyManager::valueType(propertyType);
}

QtVariantProperty *PropertyManager::addProperty(int propertyType, const QString &name)
{
	bool wasCreatingType = creatingType;

	if (propertyType == viewPropertyTypeId())
	{
		creatingType = viewPropertyTypeId();
	}
	else
	{
		creatingType = 0;
	}

	QtVariantProperty *returnVal = QtVariantPropertyManager::addProperty(propertyType, name);

	creatingType = wasCreatingType;

	return returnVal;
}

void PropertyManager::initializeProperty(QtProperty *property)
{
	if (creatingType == viewPropertyTypeId())
	{
		propsToVariants_[property] = std::move(QVariant::fromValue(ViewProperties()));
	}
	else
	{
		QtVariantPropertyManager::initializeProperty(property);
	}
}

void PropertyManager::uninitializeProperty(QtProperty *property)
{
	if (propsToVariants_.contains(property))
	{
		propsToVariants_.remove(property);
	}

	QtVariantPropertyManager::uninitializeProperty(property);
}

QVariant PropertyManager::value(const QtProperty *property) const
{
	if (propsToVariants_.contains(property))
	{
		return propsToVariants_[property];
	}
	else
	{
		return QtVariantPropertyManager::value(property);
	}
	
}

void PropertyManager::setValue(QtProperty *property, const QVariant &val)
{
	if (propsToVariants_.contains(property))
	{
		propsToVariants_[property] = val;
		emit propertyChanged(property);
		emit valueChanged(property, val);
	}
	else
	{
		QtVariantPropertyManager::setValue(property, val);
	}

}
