#include "AssetFactory.h"

using namespace Picto;
AssetFactory::AssetFactory() :
minAssets_(0),
maxAssets_(0),
newAssetFn_(NULL),
numSourcedAssets_(0)
{
}
AssetFactory::AssetFactory(QSharedPointer<Asset> singleDefaultAsset) :
minAssets_(1),
maxAssets_(1),
newAssetFn_(NULL),
numSourcedAssets_(0)
{
	defaultAssets_.push_back(singleDefaultAsset);
}

AssetFactory::AssetFactory	(	
	int minAssets,
	int maxAssets,
	NewAssetFnPtr newAssetFn, 
	QVector<QSharedPointer<Asset>> defaultAssets
									) :
minAssets_(minAssets),
maxAssets_(maxAssets),
newAssetFn_(newAssetFn),
defaultAssets_(defaultAssets),
numSourcedAssets_(0)
{
}

void AssetFactory::addAssetType(QString type,QSharedPointer<AssetFactory> factory)
{
	factoriesByType_[type] = factory;
}

QList<QString> AssetFactory::getTypes()
{
	QList<QString> returnVal;
	returnVal.push_back("");
	returnVal.append(factoriesByType_.keys());
	return returnVal;
}

QSharedPointer<Asset> AssetFactory::getAsset(QString& error, QString type)
{
	if(type != "")
	{
		if(factoriesByType_.contains(type))
			return factoriesByType_.value(type)->getAsset(error,"");
		error = "Factory does not contain type: " + type.toAscii();
		return QSharedPointer<Asset>();
	}
	if((maxAssets_ > -1) && (numSourcedAssets_ >= maxAssets_))
	{
		error = "Num  created Assets has exceeded the maximum: " + QString::number(maxAssets_);
		return QSharedPointer<Asset>();
	}
	numSourcedAssets_++;
	if(numSourcedAssets_ <= defaultAssets_.size())
		return defaultAssets_[numSourcedAssets_-1];
	return generateNewAsset();
}

QSharedPointer<Asset> AssetFactory::generateNewAsset()
{
	return QSharedPointer<Asset>((*newAssetFn_)());
}

//Resets the number of data stores sourced so far
void AssetFactory::startSourcing()
{
	numSourcedAssets_ = 0;
	foreach(QSharedPointer<AssetFactory> factory,factoriesByType_)
	{
		factory->startSourcing();
	}
}

//Returns QSharedPointers to Assets until all the requirements
//for minimum numbers of Assets in this factory have been met.
QSharedPointer<Asset> AssetFactory::getRequiredAsset()
{
	QSharedPointer<Asset> newAsset;
	foreach(QSharedPointer<AssetFactory> factory,factoriesByType_)
	{
		newAsset = factory->getRequiredAsset();
		if(!newAsset.isNull())
			return newAsset;
	}
	if(numSourcedAssets_ < minAssets_)
		newAsset = getAsset(QString(""));
	return newAsset;
}

