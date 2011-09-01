#include "PropertyTable.h"

using namespace Picto;

PropertyTable::PropertyTable()
{
}
void PropertyTable::addProperty(QSharedPointer<Property> prop)
{
	Q_ASSERT(prop);
				QString pName = prop->getName();
			QString path = prop->getPath();
	prop->setIndex(propTable_.size());
	propTable_.append(prop);
	connect(prop.data(),SIGNAL(valueChanged(QSharedPointer<Property>)),this,SIGNAL(propertyChanged(QSharedPointer<Property>)));
}

void PropertyTable::updatePropertyValue(int index,QString value)
{
	Q_ASSERT(index < propTable_.size());
	QSharedPointer<Property> prop = propTable_[index];
	prop->fromXml(value);
}