#include "ControlElement.h"
#include "../statemachine/LogicResult.h"
#include "../storage/ObsoleteAsset.h"

namespace Picto {

ControlElement::ControlElement()
{
	//Control elements don't contain transitions.  All of their results are logic results which differ
	//from regular results in that they don't have scripts.  Replace the default result factory with
	//a factory that creates LogicResults that are required.
	defineResultFactoryType("",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LogicResult::Create))));
	AddDefinableObjectFactory("Type",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
}

//bool ControlElement::addResult(QSharedPointer<Result> result)
//{
//	results_[result->name()] = result;
//	QList<QSharedPointer<Asset>> resultChildren = getGeneratedChildren("Result");
//	bool found = false;
//	foreach(QSharedPointer<Asset> resultChild,resultChildren)
//	{
//		if(resultChild->name() == result->name())
//		{
//			found = true;
//			break;
//		}
//	}
//	if(!found)
//		AddChild("Result",result);
//	return true;
//}
//
//QSharedPointer<Result> ControlElement::getResult(QString name)
//{
//	if(results_.contains(name))
//		return results_[name];
//	return QSharedPointer<Result>();
//}

void ControlElement::postDeserialize()
{
	ResultContainer::postDeserialize();
	////Re-Add all default results as children since serialization clears the child list.
	//QStringList resultKeys = results_.keys();
	//foreach(QString resultKey,resultKeys)
	//{
	//	AddChild("Result",results_[resultKey]);
	//}
}

bool ControlElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ResultContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
