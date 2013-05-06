#include "VarMap.h"
#include "../memleakdetect.h"

namespace Picto {

VarMap::VarMap()
: Variable()
{
	map_.clear();
	AddDefinableProperty(QVariant::Map,"Value",QVariant());
}

QSharedPointer<Asset> VarMap::Create()
{
	return QSharedPointer<Asset>(new VarMap());
}

void VarMap::reset()
{
	Variable::reset();
	map_ = propertyContainer_->getPropertyValue("Value").toMap();
}

int VarMap::length()
{
	return map_.size();
}

void VarMap::setValue(QVariant key, QVariant value)
{
	map_[key.toString()] = value;
	propertyContainer_->setPropertyValue("Value",map_);
}

void VarMap::fromAssocArray(QVariantMap obj)
{
	map_ = obj;
	propertyContainer_->setPropertyValue("Value",map_);
}

QVariant VarMap::getValue(QVariant key)
{
	QString keyString = key.toString();
	if(!map_.contains(keyString))
		return QVariant();
	return map_.value(keyString);
}

QVariantList VarMap::getKeys()
{
	QVariantList returnVal;
	returnVal.reserve(map_.size());
	foreach(QString key,map_.keys())
	{
		returnVal.append(key);
	}
	return returnVal;
}

QVariantList VarMap::getValues()
{
	return map_.values();
}

QVariantMap VarMap::toAssocArray()
{
	return map_;
}

QVariant VarMap::takeAt(QVariant key)
{
	QVariant returnVal = map_.take(key.toString());
	propertyContainer_->setPropertyValue("Value",map_);
	return returnVal;
}
	
void VarMap::removeAt(QVariant key)
{
	map_.take(key.toString());
	propertyContainer_->setPropertyValue("Value",map_);
}

void VarMap::postDeserialize()
{
	Variable::postDeserialize();
	propertyContainer_->getProperty("Value")->setVisible(false);
}

bool VarMap::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Variable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
