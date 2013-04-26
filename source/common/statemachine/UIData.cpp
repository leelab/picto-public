#include "UIData.h"
#include "UIElement.h"
#include "../design/PictoData.h"
#include "../experiment/Experiment.h"
#include "../parameter/Analysis.h"
#include "../memleakdetect.h"

namespace Picto {

UIData::UIData()
:	AssociateRoot(),
	elementFactory_(new AssetFactory(0,-1))
{
	AddDefinableProperty(QVariant::Int,"OpenAsset",0);

	AddDefinableObjectFactory("UIElement",elementFactory_);
	elementFactory_->addAssetType("",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(UIElement::Create))));
}

QSharedPointer<Asset> UIData::Create()
{
	QSharedPointer<UIData> newUIData(new UIData());
	newUIData->setSelfPtr(newUIData);
	return newUIData;
}

void UIData::setOpenAsset(int assetId)
{
	propertyContainer_->setPropertyValue("OpenAsset",assetId);
}

int UIData::getOpenAsset()
{
	return propertyContainer_->getPropertyValue("OpenAsset").toInt();
}

void UIData::postDeserialize()
{
	AssociateRoot::postDeserialize();
}

bool UIData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AssociateRoot::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

