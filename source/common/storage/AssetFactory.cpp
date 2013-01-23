#include "AssetFactory.h"
#include "../statemachine/uienabled.h"
#include "../memleakdetect.h"

using namespace Picto;
AssetFactory::AssetFactory(int minAssets,int maxAssets ) :
minAssets_(minAssets),
maxAssets_(maxAssets),
newAssetFn_(NULL),
numSourcedAssets_(0),
isGroupFactory_(true),
uITemplateInitialized_(false)
{
}

AssetFactory::AssetFactory	( 
	int minAssets,
	int maxAssets,
	NewAssetFnPtr newAssetFn
									) :
minAssets_(minAssets),
maxAssets_(maxAssets),
newAssetFn_(newAssetFn),
numSourcedAssets_(0),
isGroupFactory_(false),
uITemplateInitialized_(false)
{
}

void AssetFactory::addAssetType(QString type,QSharedPointer<AssetFactory> factory)
{
	Q_ASSERT(factory.data() != this);
	Q_ASSERT_X(isGroupFactory_,"AssetFactory::addAssetType","Asset Types can only be added to AssetFactories created with the AssetFactory(int minAssets,int maxAssets) constructor");
	factoriesByType_[type] = factory;
}

QStringList AssetFactory::getTypes()
{
	QList<QString> returnVal;
	if(isGroupFactory_)
		returnVal.append(factoriesByType_.keys());
	else
		returnVal.append("");
	return returnVal;
}

QSharedPointer<Asset> AssetFactory::getAsset(QString& error, QString type)
{
	QSharedPointer<Asset> returnVal;
	if((maxAssets_ > -1) && (numSourcedAssets_ >= maxAssets_))
	{
		error = "Num  created Assets has exceeded the maximum: " + QString::number(maxAssets_);
		return returnVal;
	}
	if(isGroupFactory_)
	{
		if(factoriesByType_.contains(type))
		{
			returnVal = factoriesByType_.value(type)->getAsset(error,"");
		}
		else
		{
			error = "Factory does not contain type: " + type.toLatin1();
			return returnVal;
		}
	}
	else
	{
		returnVal = generateNewAsset();
		if(returnVal && !uITemplateInitialized_)
		{
			if(returnVal->inherits("Picto::UIEnabled"))
				uITemplate_ = returnVal.staticCast<UIEnabled>()->getUITemplate();
			else
				uITemplate_ = "";
			uITemplateInitialized_ = true;
		}
	}
	if(!returnVal.isNull())
	{
		numSourcedAssets_++;
		connect(returnVal.data(),SIGNAL(destroyed(QObject*)),this,SLOT(createdAssetDestroyed(QObject*)));
	}
	return returnVal;
}

QSharedPointer<Asset> AssetFactory::generateNewAsset()
{
	Q_ASSERT(!isGroupFactory_);
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
QSharedPointer<Asset> AssetFactory::getRequiredAsset(QString& returnedType)
{
	returnedType = "";
	QSharedPointer<Asset> newAsset;
	if(numSourcedAssets_ >= minAssets_)
	{
		return newAsset;
	}

	if(!isGroupFactory_)
	{
		newAsset = getAsset(QString(""));
		return newAsset;
	}

	for(QMap<QString, QSharedPointer<AssetFactory>>::iterator iter = factoriesByType_.begin();
		iter != factoriesByType_.end();iter++)
	{
		QString subRetType = "";
		newAsset = (iter.value())->getRequiredAsset(subRetType);
		if(!newAsset.isNull())
		{
			if(subRetType == "")
				returnedType = iter.key();
			else
				returnedType = subRetType;
			numSourcedAssets_++;
			return newAsset;
		}
	}

	QStringList types = getTypes();
	foreach(QString type,types)
	{
		newAsset = getAsset(QString(""),type);
		if(!newAsset.isNull())
		{
			returnedType = type;
			return newAsset;
		}
	}
	Q_ASSERT_X(true,"AssetFactory::getRequiredAsset","This asset factory cannot produce the minimum number of required assets as it is currently defined.");
	return newAsset;
}

//Returns true if no more assets of the input type can be produced by this
//factory.
bool AssetFactory::reachedProductionLimit(QString type)
{
	if(numSourcedAssets_ == maxAssets_)
		return true;
	if(isGroupFactory_)
	{
		if(!factoriesByType_.contains(type))
			return true;
		return factoriesByType_[type]->reachedProductionLimit();
	}
	else if(type != "")
		return true;
	return false;
}

QString AssetFactory::getUITemplate(QString type)
{
	if(isGroupFactory_)
	{
		Q_ASSERT_X(factoriesByType_.contains(type),"AssetFactory::getUITemplate","This factory does not contain type: " + type.toLatin1());
		return factoriesByType_[type]->getUITemplate("");
	}
	//If uITemplate has not yet been initialized, create and throw out an asset so that it will be.
	if(!uITemplateInitialized_)
		getAsset(QString(""),"");
	return uITemplate_;
}

/*! \brief Decreases the numSourcedAssets counter whenever an asset created by this factory is deleted.
 */
void AssetFactory::createdAssetDestroyed(QObject*)
{
	numSourcedAssets_--;
}