#include "PropertyTable.h"

using namespace Picto;

PropertyTable::PropertyTable(QSharedPointer<ExperimentConfig> expConfig)
: expConfig_(expConfig)
{
	Q_ASSERT(expConfig_);
}
void PropertyTable::addProperty(QSharedPointer<Property> prop)
{
	connect(prop.data(),SIGNAL(valueChanged(QSharedPointer<Property>)),this,SIGNAL(propertyChanged(QSharedPointer<Property>)));
}

void PropertyTable::updatePropertyValue(int index,QString value)
{
	QSharedPointer<Asset> asset = expConfig_->getAsset(index);
	Q_ASSERT(asset->inherits("Picto::Property"));
	asset.staticCast<Property>()->fromUserString(value);
	//Q_ASSERT(index < propTable_.size());
	//QSharedPointer<Property> prop = propTable_[index];
	//prop->fromUserString(value);
}

//This can be used in conjunction with the Picto Engine to send all current property values to server.
//Note that this function is set to ignore UIInfo objects, Transitions and Control Links.  Transitions and
//Control Links don't have any properties that can change during and experiment.  UIInfo objects have no 
//bearing on the experiment and are used only for UI information for the State Machine Editor.
void PropertyTable::reportChangeInAllProperties()
{
	foreach(QWeakPointer<Asset> prop,expConfig_->getProperties())
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
		emit propertyChanged(sAsset.staticCast<Property>());
	}
}

//QSharedPointer<PropertyLookup> PropertyTable::getPropLookup()
//{
//	if(!propLookup_)
//	{
//		propLookup_ = QSharedPointer<PropertyLookup>(new PropertyLookup());
//	}
//	propLookup_->clear();
//	foreach(QSharedPointer<Property> prop,propTable_)
//	{
//		propLookup_->addProperty(prop);
//	}
//	return propLookup_;
//}