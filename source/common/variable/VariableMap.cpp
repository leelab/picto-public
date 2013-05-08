#include "VariableMap.h"
#include "../memleakdetect.h"

namespace Picto {

VariableMap::VariableMap()
: Variable()
{
	map_.clear();
	AddDefinableProperty(QVariant::Map,"Value",QVariant());
}

QSharedPointer<Asset> VariableMap::Create()
{
	return QSharedPointer<Asset>(new VariableMap());
}

void VariableMap::reset()
{
	Variable::reset();
	map_ = propertyContainer_->getPropertyValue("Value").toMap();
}

int VariableMap::length()
{
	return map_.size();
}

void VariableMap::setValue(QVariant key, QVariant value)
{
	map_[key.toString()] = value;
	propertyContainer_->setPropertyValue("Value",map_);
}

void VariableMap::fromAssocArray(QVariantMap assocArray)
{
	//Is this a problem because the keys might not be strings?
	map_ = assocArray;
	propertyContainer_->setPropertyValue("Value",map_);
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
	propertyContainer_->setPropertyValue("Value",map_);
}

void VariableMap::postDeserialize()
{
	Variable::postDeserialize();
	propertyContainer_->getProperty("Value")->setVisible(false);
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
	propertyContainer_->setPropertyValue("Value",map_);
	return returnVal;
}

}; //namespace Picto
