#include "ResultContainer.h"

using namespace Picto;

ResultContainer::ResultContainer()
: resultFactory_(new AssetFactory(0,-1))
{
	addResultFactoryType("",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Result::Create))));
	AddDefinableObjectFactory("Result",resultFactory_);
}

//! \brief Returns a list of all possible results.
QStringList ResultContainer::getResultList()
{
	return results_.keys();
}

QSharedPointer<Result> ResultContainer::getResult(QString name)
{
	if(results_.contains(name))
		return results_[name];
	return QSharedPointer<Result>();
}

void ResultContainer::addRequiredResult(QString resultName)
{
	if(maxOptionalResults_[""] != -1)
	{
		//This will increase the max results value in the factory according to 
		//the new number of required results of this type.  If we didn't do
		//this, we couldn't get a new result from the factory below.
		//Note: if we didn't add the new result to requiredResults_ below, this
		//would make the factories max Assets inconsistant with requireResults_.
		resultFactoryByType_[""]->setMaxAssets(resultFactoryByType_[""]->getMaxAssets()+1);
	}
	QSharedPointer<Result> requiredResult = createChildAsset("Result","",QString()).staticCast<Result>();
	Q_ASSERT(requiredResult);
	requiredResult->setName(resultName);
	requiredResults_.insert("",requiredResult);
}

//void ResultContainer::addRequiredResult(QSharedPointer<Result> requiredResult, QString type)
//{
//	Q_ASSERT_X(resultFactoryByType_.contains(type),"ResultContainer",
//		QString("You must add an asset factory for type: \"%1\" before calling addRequiredResult"
//		" for that type.").arg(type).toAscii());
//	requiredResults_.insert(type,requiredResult);
//	if(maxOptionalResults_[type] != -1)
//	{
//		//This will reset the value in the factory according to the new number of required results of this type
//		setMaxOptionalResults(maxOptionalResults_[type],type);
//	}
//}

void ResultContainer::setMaxOptionalResults(int max, QString type)
{
	Q_ASSERT_X(resultFactoryByType_.contains(type),"ResultContainer",
		QString("You must add an asset factory for type: \"%1\" before calling setMaxOptionalResults"
		" for that type.").arg(type).toAscii());
	maxOptionalResults_[type] = max;
	resultFactoryByType_[type]->setMaxAssets(max+requiredResults_.count(type));
}

void ResultContainer::addResultFactoryType(QString type,QSharedPointer<AssetFactory> resultFactory)
{
	Q_ASSERT_X(!resultFactoryByType_.contains(type),"ResultContainer",
		QString("The factory type \"%1\" was added to this ResultContainer twice").arg(type).toAscii());
	resultFactoryByType_[type] = resultFactory;
	resultFactory_->addAssetType(type,resultFactory);
	maxOptionalResults_[type] = -1;
}

void ResultContainer::postSerialize()
{
	ScriptableContainer::postSerialize();
	//Check child list to see if any of the requiredResults were serialized in, if not, add them to the child
	//list.
	QList<QSharedPointer<Asset>> deserializedResults = getGeneratedChildren("Result");
	for(QMultiMap<QString,QSharedPointer<Result>>::iterator it = requiredResults_.begin();it!=requiredResults_.end();it++)
	{
		bool found = false;
		foreach(QSharedPointer<Asset> deserializedResult,deserializedResults)
		{
			if(deserializedResult->getName() == it.value()->getName())
			{
				found = true;
				break;
			}
		}
		if(!found)
		{
			AddChild("Result",it.value());
			//Since the requiredResults weren't generated by the factory, we need to increment the number
			//of generated results so that the factory will no if it can generate any more of the given type.
			resultFactoryByType_[it.key()]->setGeneratedAssets(resultFactoryByType_[it.key()]->getGeneratedAssets()+1);
		}
	}

	//Add all child results to results_ map.
	results_.clear();
	QList<QSharedPointer<Asset>> results = getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> result,results)
	{
		results_[result->getName()] = result.staticCast<Result>();
	}

	////Re-Add all default results.  This puts them into the child map if they weren't serialized in.
	////(They went into the child map the first time they were added too, but then got erased during serialize).
	//QMap<QString, QSharedPointer<Result> > localResults = results_;
	//foreach(QSharedPointer<Result> result,localResults)
	//{
	//	addResult(result);
	//}

	////Put all child results that were serialized in into the results list.
	//updateResultsFromChildren();
}

bool ResultContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//! \todo verify that no two results have the same name
	if(!ScriptableContainer::validateObject(xmlStreamReader))
		return false;
	if( (resultFactory_->getMaxAssets() != -1)
		&& getGeneratedChildren("Result").size() > resultFactory_->getMaxAssets())
	{
		QString errorStr = QString("This ResultContainer may only contain a maximum of %1 results including the followind required assets:\n").arg(resultFactory_->getMaxAssets());
		foreach(QSharedPointer<Result> result,requiredResults_)
		{
			errorStr.append(QString("%1\n").arg(result->getName()));
		}
		addError("ResultContainer",errorStr, xmlStreamReader);
		return false;
	}
	return true;
}

////Put all child results that were serialized in into the results list.
//void ResultContainer::updateResultsFromChildren()
//{
//	results_.clear();
//	QList<QSharedPointer<Asset>> results = getGeneratedChildren("Result");
//	foreach(QSharedPointer<Asset> result,results)
//	{
//		addResult(result.staticCast<Result>());
//	}
//}