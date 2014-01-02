#include "AssetFactory.h"
#include "../statemachine/uienabled.h"
#include "../memleakdetect.h"

using namespace Picto;
/*! \brief Creates an AssetFactory object.
 *	@param minAssets The minimum number of Assets that need to be generated from this factory.
 *	@param maxAssets The maximum number of Assets that may be generated from this factory (-1 means no limit).
 */
AssetFactory::AssetFactory(int minAssets,int maxAssets ) :
minAssets_(minAssets),
maxAssets_(maxAssets),
newAssetFn_(NULL),
numSourcedAssets_(0),
isGroupFactory_(true),
uITemplateInitialized_(false)
{
}

/*! \brief Creates an AssetFactory object.
 *	@param minAssets The minimum number of Assets that need to be generated from this factory.
 *	@param maxAssets The maximum number of Assets that may be generated from this factory.
 *	@param newAssetFn A pointer to the function that will be used by this factory to create
 *		new Assets.
 */
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

/*! \brief Adds a sub-AssetFactory to this object to handle creation of Assets of the input type.
 *	\details Calling this function generates a lookup table of sub-AssetFactory objects that
 *	handle Asset creation when getAsset() is called with a type input that matches their type.
 *	\note When sub-AssetFactories are used.  Both their minAssets/maxAssets values must
 *	be met as well as those of the parent AssetFactory.  This means, for example, that if the
 *	parant AssetFactory has a maximum of 5 Assets, all Assets from all
 *	sub-Factores added together must be at least 5.  It is your job to make sure that the
 *	total maxAsset/minAsset values of sub-AssetFactories and the parent AssetFactory are logically
 *	consisten.  Also note that AssetFactories with sub-AssetFactories cannot create their own Assets. 
 *	These types of AssetFactories are constructed without the newAssetFnPtr input.
 */
void AssetFactory::addAssetType(QString type,QSharedPointer<AssetFactory> factory)
{
	Q_ASSERT(factory.data() != this);
	Q_ASSERT_X(isGroupFactory_,"AssetFactory::addAssetType","Asset Types can only be added to AssetFactories created with the AssetFactory(int minAssets,int maxAssets) constructor");
	factoriesByType_[type] = factory;
}

/*! \brief Returns the types of sub-Asset Factories in this AssetFactory.
 *	\details If this isn't a "Group" AssetFactory, the StringList will only contain one
 *	value: an empty string.
*/
QStringList AssetFactory::getTypes()
{
	QList<QString> returnVal;
	if(isGroupFactory_)
		returnVal.append(factoriesByType_.keys());
	else
		returnVal.append("");
	return returnVal;
}

/*! \brief Creates a new Asset of the input type.  If there is a problem generating the Asset, 
 *	 a descriptive error string is returned in the error reference input.
 *	\details If this is a "Group" AssetFactory, sub-AssetFactory objects are used to construct
 *	the Asset.  Otherwise, generateNewAsset() generates the Asset.  
 *	\note This function also counts the number of Assets that it generates.  Whenever it generates
 *	an Asset, it connects to its destroyed() signals so that it can track when the Asset is
 *	deleted.  If a generated Asset is deleted, it frees up another Asset generation in cases where there
 *	is a maximum allowed number of Assets.
 */
QSharedPointer<Asset> AssetFactory::getAsset(QString& error, QString type)
{
	QSharedPointer<Asset> returnVal;
	if((maxAssets_ > -1) && (numSourcedAssets_ >= maxAssets_))
	{
		error = "Number of created Assets has exceeded the maximum: " + QString::number(maxAssets_);
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
			{
				uITemplate_ = returnVal.staticCast<UIEnabled>()->getUITemplate();
				uIGroup_ = returnVal.staticCast<UIEnabled>()->getUIGroup();
				generatedAssetTypeName_ = returnVal.staticCast<UIEnabled>()->friendlyTypeName();
				assetClassName_ = returnVal->metaObject()->className();
				assetClassName_.replace("Picto::","");
			}
			else
			{
				generatedAssetTypeName_ = "";
				uITemplate_ = "";
				assetClassName_ = "";
				uIGroup_ = "";
			}
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

/*! \brief Generates a new Asset using the creation function pointer and returns a shared pointer to it.
*/
QSharedPointer<Asset> AssetFactory::generateNewAsset()
{
	Q_ASSERT(!isGroupFactory_);
	return QSharedPointer<Asset>((*newAssetFn_)());
}

/*! \brief Initializes Asset generation by resetting the number of Assets sourced so far.
*/
void AssetFactory::startSourcing()
{
	numSourcedAssets_ = 0;
	foreach(QSharedPointer<AssetFactory> factory,factoriesByType_)
	{
		factory->startSourcing();
	}
}

/*! \brief Call this function repeatedly until it returns an empty pointer to generate the required
 *	minimum	number of Assets for this Factory.
 *	\details Returns a string with the type of the generated Asset in the returnedType reference parameter.
 *	\sa addAssetType()
 */
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

/* \brief Returns true if no more Assets of the input type can be produced by this factory (since the
 *	maxAssets value was reached).
 */
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

/*! \brief Returns the maximum allowed number of Assets for the input type.
 *	\details An empty type causes the function to return this Factory's (not a sub-Factory)
 *	maximum number of Assets.
*/
int AssetFactory::getMaxAssets(QString type)
{
	if(isGroupFactory_)
	{
		if(!factoriesByType_.contains(type))
			return 0;
		return factoriesByType_[type]->getMaxAssets();
	}
	else if(type != "")
		return 0;
	return maxAssets_;
}

/*! \brief Returns the minimum allowed number of Assets for the input type.
 *	\details An empty type causes the function to return this Factory's (not a sub-Factory)
 *	minimum number of Assets.
*/
int AssetFactory::getMinAssets(QString type)
{
	if(isGroupFactory_)
	{
		if(!factoriesByType_.contains(type))
			return 0;
		return factoriesByType_[type]->getMinAssets();
	}
	else if(type != "")
		return 0;
	return minAssets_;
}

/*! \brief Returns the name of the UITemplate to be used by the GUI for Assets of the input type.
 *	\details A UITemplate defines the icon characteristics for the Asset type.
 *	\details An empty type refers to this AssetFactory (not a sub-factory).
 */
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

/*! \brief Returns the name of the UIGroup to be used by the GUI for Assets of the input type.
 *	\details A UIGroup defines the section of a GUI toolbox in which generation buttons for 
 *	Assets of this type will appear.
 *	\details An empty type refers to this AssetFactory (not a sub-factory).
 */
QString AssetFactory::getUIGroup(QString type)
{
	if(isGroupFactory_)
	{
		Q_ASSERT_X(factoriesByType_.contains(type),"AssetFactory::getUITemplate","This factory does not contain type: " + type.toLatin1());
		return factoriesByType_[type]->getUIGroup("");
	}
	//If uITemplate has not yet been initialized, create and throw out an asset so that it will be.
	if(!uITemplateInitialized_)
		getAsset(QString(""),"");
	return uIGroup_;
}
/*! \brief Returns a friendly name to be used by the GUI to name Assets of the input type.
 *	\details An empty type refers to this AssetFactory (not a sub-factory).
 */
QString AssetFactory::getGeneratedAssetTypeName(QString type)
{
	//Call getUITemplate to initialize generatedAssetTypeName_ and return it.
	getUITemplate(type);
	if(isGroupFactory_)
	{
		Q_ASSERT_X(factoriesByType_.contains(type),"AssetFactory::getUITemplate","This factory does not contain type: " + type.toLatin1());
		return factoriesByType_[type]->getGeneratedAssetTypeName("");
	}
	return generatedAssetTypeName_;
}

/*! \brief Returns the name of the class defining Assets of the input type.
 *	\details This is useful for accessing the correct documentation for this specific type of Asset.
 *	\details An empty type refers to this AssetFactory (not a sub-factory).
 */
QString AssetFactory::getGeneratedAssetClassName(QString type)
{
	//Call getUITemplate to initialize assetClassName_ and return it.
	getUITemplate(type);
	if(isGroupFactory_)
	{
		Q_ASSERT_X(factoriesByType_.contains(type),"AssetFactory::getUITemplate","This factory does not contain type: " + type.toLatin1());
		return factoriesByType_[type]->getGeneratedAssetClassName("");
	}
	return assetClassName_;
}

/*! \brief Decreases the numSourcedAssets counter whenever an asset created by this factory is deleted
 *	this allows the AssetFactory to create an additional Asset if the maxAssets value has been reached.
 */
void AssetFactory::createdAssetDestroyed(QObject*)
{
	numSourcedAssets_--;
}