#include "VariableMap.h"
#include "../memleakdetect.h"

namespace Picto {

VariableMap::VariableMap()
: Variable(),
settingValueProp_(false)
{
	map_.clear();
	AddDefinableProperty(QVariant::Map,"Value",QVariant());
}

QSharedPointer<Asset> VariableMap::Create()
{
	return QSharedPointer<Asset>(new VariableMap());
}

int VariableMap::length()
{
	return map_.size();
}

void VariableMap::setValue(QVariant key, QVariant value)
{
	map_[key.toString()] = value;
	copyMapToValueProp();
}

void VariableMap::fromAssocArray(QVariantMap assocArray)
{
	//Is this a problem because the keys might not be strings?
	map_ = assocArray;
	copyMapToValueProp();
}

QVariantList VariableMap::getKeys()
{
	QVariantList returnVal;
	returnVal.reserve(map_.size());
	foreach(QString key,map_.keys())
	{
		returnVal.append(key);
	}
	return returnVal;
}
	
void VariableMap::removeAt(QVariant key)
{
	map_.take(key.toString());
	copyMapToValueProp();
}

void VariableMap::postDeserialize()
{
	Variable::postDeserialize();
	propertyContainer_->getProperty("Value")->setVisible(false);
	connect(propertyContainer_->getProperty("Value").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propValueChanged(Property*,QVariant)));
}

bool VariableMap::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Variable::validateObject(xmlStreamReader))
		return false;
	return true;
}

QVariant VariableMap::getValue(QVariant key)
{
	QString keyString = key.toString();
	if(!map_.contains(keyString))
		return QVariant();
	return map_.value(keyString);
}

QVariant VariableMap::takeAt(QVariant key)
{
	QVariant returnVal = map_.take(key.toString());
	copyMapToValueProp();
	return returnVal;
}

void VariableMap::copyMapToValueProp()
{
	settingValueProp_ = true;
	propertyContainer_->setPropertyValue("Value",map_);
	settingValueProp_ = false;
}

void VariableMap::copyValuePropToMap()
{
	map_ = propertyContainer_->getPropertyValue("Value").toMap();
}

void VariableMap::propValueChanged(Property*,QVariant)
{
	//If the change happened because of one of this object's functions, return.
	if(settingValueProp_)
		return;
	//The change must have happened from an outside source (ie. The value was reset to the initValue
	//or this is a slave and something changed on the master)
	//Set our Map from the Value property
	copyValuePropToMap();
}

}; //namespace Picto
