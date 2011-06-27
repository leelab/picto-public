#include "ParameterContainer.h"
#include "ParameterFactory.h"
#include "BooleanParameter.h"
#include "ChoiceParameter.h"
#include "NumericParameter.h"
#include "RangeParameter.h"

namespace Picto {

ParameterContainer::ParameterContainer()
{
	scriptableFactory_->addAssetType("Boolean",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BooleanParameter::Create))));
	scriptableFactory_->addAssetType("Choice",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ChoiceParameter::Create))));
	scriptableFactory_->addAssetType("Numeric",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(NumericParameter::Create))));
	scriptableFactory_->addAssetType("Range",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RangeParameter::Create))));

}

//QSharedPointer<Parameter> ParameterContainer::getParameter(QString name)
//{
//	return getScriptable(name).staticCast<Parameter>();
//}

void ParameterContainer::postSerialize()
{
	ResultContainer::postSerialize();
}

bool ParameterContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ResultContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
