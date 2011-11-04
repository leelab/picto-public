#include "UIEnabled.h"
#include "../statemachine/uiinfo.h"
namespace Picto
{

UIEnabled::UIEnabled()
: DataStore()
{
	AddDefinableProperty("Name","Not Yet Named");
	AddDefinableObjectFactory("UIInfo",
			QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(UIInfo::Create))));
}

void UIEnabled::setPos(QPoint pos)
{
	getGeneratedChildren("UIInfo").first().staticCast<UIInfo>()->setPos(pos);
}

QPoint UIEnabled::getPos()
{
	return getGeneratedChildren("UIInfo").first().staticCast<UIInfo>()->getPos();
}

QUuid UIEnabled::getUniqueId()
{
	return getGeneratedChildren("UIInfo").first().staticCast<UIInfo>()->getUniqueId();
}

void UIEnabled::postDeserialize()
{
	DataStore::postDeserialize();
	connect(propertyContainer_->getProperty("Name").data(),SIGNAL(edited()),this,SIGNAL(nameEdited()));
}


bool UIEnabled::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto