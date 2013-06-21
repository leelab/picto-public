#include "AnalysisVariableMap.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisVariableMap::AnalysisVariableMap()
: AnalysisVariable()
{
	map_.clear();
	AddDefinableObjectFactory("Value",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
}

QSharedPointer<Asset> AnalysisVariableMap::Create()
{
	return QSharedPointer<Asset>(new AnalysisVariableMap());
}

void AnalysisVariableMap::reset()
{
	AnalysisVariable::reset();
	map_.clear();
}

int AnalysisVariableMap::length()
{
	return map_.size();
}

void AnalysisVariableMap::setValue(QVariant key, QVariant value)
{
	map_[key.toString()] = value;
}

void AnalysisVariableMap::fromAssocArray(QVariantMap assocArray)
{
	//Is this a problem because the keys might not be strings?
	map_ = assocArray;
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
}

void AnalysisVariableMap::postDeserialize()
{
	AnalysisVariable::postDeserialize();
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
	return returnVal;
}

}; //namespace Picto
