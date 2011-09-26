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
	prop->fromUserString(value);
}

//This can be used in conjunction with the Picto Engine to send all current property values to server.
//Note that this function is set to ignore UIInfo objects, Transitions and Control Links.  Transitions and
//Control Links don't have any properties that can change during and experiment.  UIInfo objects have no 
//bearing on the experiment and are used only for UI information for the State Machine Editor.
void PropertyTable::reportChangeInAllProperties()
{
	foreach(QSharedPointer<Property> prop,propTable_)
	{
		QSharedPointer<Asset> propParent = prop->getParentAsset();
		if(	propParent 
			&& (	
				propParent->inherits("Picto::Transition") 
				||	propParent->inherits("Picto::ControlLink")
				||	propParent->inherits("Picto::UIInfo")
			)
		)
		continue;
		emit propertyChanged(prop);
	}
}