#include "AnalysisVariableMap.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisVariableMap::AnalysisVariableMap()
: AnalysisVariable(),
settingValueProp_(false)
{
	map_.clear();
	AddDefinableProperty(QVariant::Map,"Value",QVariant());
}

QSharedPointer<Asset> AnalysisVariableMap::Create()
{
	return QSharedPointer<Asset>(new AnalysisVariableMap());
}

int AnalysisVariableMap::length()
{
	return map_.size();
}

void AnalysisVariableMap::setValue(QVariant key, QVariant value)
{
	map_[key.toString()] = value;
	copyMapToValueProp();
}

void AnalysisVariableMap::fromAssocArray(QVariantMap assocArray)
{
	//Is this a problem because the keys might not be strings?
	map_ = assocArray;
	copyMapToValueProp();
}

QVariantList AnalysisVariableMap::getKeys()
{
	QVariantList returnVal;
	returnVal.reserve(map_.size());
	foreach(QString key,map_.keys())
	{
		returnVal.append(key);
	}
	return returnVal;
}
	
void AnalysisVariableMap::removeAt(QVariant key)
{
	map_.take(key.toString());
	copyMapToValueProp();
}

void AnalysisVariableMap::postDeserialize()
{
	AnalysisVariable::postDeserialize();
	propertyContainer_->getProperty("Value")->setVisible(false);
	connect(propertyContainer_->getProperty("Value").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propValueChanged(Property*,QVariant)));
}

bool AnalysisVariableMap::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}

QVariant AnalysisVariableMap::getValue(QVariant key)
{
	QString keyString = key.toString();
	if(!map_.contains(keyString))
		return QVariant();
	return map_.value(keyString);
}

QVariant AnalysisVariableMap::takeAt(QVariant key)
{
	QVariant returnVal = map_.take(key.toString());
	copyMapToValueProp();
	return returnVal;
}

void AnalysisVariableMap::copyMapToValueProp()
{
	settingValueProp_ = true;
	propertyContainer_->setPropertyValue("Value",map_);
	settingValueProp_ = false;
}

void AnalysisVariableMap::copyValuePropToMap()
{
	map_ = propertyContainer_->getPropertyValue("Value").toMap();
}

void AnalysisVariableMap::propValueChanged(Property*,QVariant)
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
