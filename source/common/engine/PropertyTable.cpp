#include "PropertyTable.h"
#include "../memleakdetect.h"

using namespace Picto;

PropertyTable::PropertyTable(QSharedPointer<DesignConfig> designConfig)
: designConfig_(designConfig)
{
	Q_ASSERT(designConfig_);
}
void PropertyTable::addProperty(QSharedPointer<Property> prop)
{
	connect(prop.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propValueChange(Property*,QVariant)));
	connect(prop.data(),SIGNAL(initValueChanged(Property*,QVariant)),this,SLOT(propInitValueChange(Property*,QVariant)));
}

//This can be used in conjunction with the Picto Engine to send all current property values to server.
//Note that this function is set to ignore UIInfo objects, Transitions and Control Links.  Transitions and
//Control Links don't have any properties that can change during and experiment.  UIInfo objects have no 
//bearing on the experiment and are used only for UI information for the State Machine Editor.
void PropertyTable::reportChangeInAllProperties()
{
	foreach(QWeakPointer<Asset> prop,designConfig_->getProperties())
	{
		QSharedPointer<Asset> sAsset(prop);
		Q_ASSERT(sAsset);
		QSharedPointer<Asset> propParent = sAsset->getParentAsset();
		if(	propParent 
			&& (	
				propParent->inherits("Picto::Transition") 
				||	propParent->inherits("Picto::ControlLink")
				||	propParent->inherits("Picto::UIInfo")
			)
		)
		continue;
		emit propertyValueChanged(sAsset.staticCast<Property>().data());
		emit propertyInitValueChanged(sAsset.staticCast<Property>().data());
	}
}

void PropertyTable::updateInitPropertyValue(int index,QString value)
{
	QSharedPointer<Asset> asset = designConfig_->getAsset(index);
	Q_ASSERT(asset->inherits("Picto::Property"));
	asset.staticCast<Property>()->initValFromUserString(value);
}

void PropertyTable::propValueChange(Property* changedProp,QVariant)
{
	emit propertyValueChanged(changedProp);
}

void PropertyTable::propInitValueChange(Property* changedProp,QVariant)
{
	emit propertyInitValueChanged(changedProp);
}
