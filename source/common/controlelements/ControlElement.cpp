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

void ControlElement::postDeserialize()
{
	ResultContainer::postDeserialize();
}

bool ControlElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ResultContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
