#include "UIEnabled.h"
#include "../statemachine/uiinfo.h"
#include "../memleakdetect.h"
namespace Picto
{

UIEnabled::UIEnabled()
: DataStore()
{
	AddDefinableProperty("Name","Not Yet Named");
	AddDefinableObjectFactory("UIInfo",
			QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(UIInfo::Create))));
}

QSharedPointer<Asset> UIEnabled::Create()
{
	return QSharedPointer<Asset>(new UIEnabled());
}

void UIEnabled::setPos(QPoint pos)
{
	getGeneratedChildren("UIInfo").first().staticCast<UIInfo>()->setPos(pos);
}

QPoint UIEnabled::getPos()
{
	return getGeneratedChildren("UIInfo").first().staticCast<UIInfo>()->getPos();
}

void UIEnabled::setOpenDescendant(int assetId)
{
	getGeneratedChildren("UIInfo").first().staticCast<UIInfo>()->setOpenDescendant(assetId);
}

int UIEnabled::getOpenDescendant()
{
	return getGeneratedChildren("UIInfo").first().staticCast<UIInfo>()->getOpenDescendant();
}

bool UIEnabled::searchForQuery(SearchRequest searchRequest)
{
	if(DataStore::searchForQuery(searchRequest))
		return true;
	switch(searchRequest.type)
	{
	case SearchRequest::STRING:
		{
			//Check if my name includes the search string
			if(getName().contains(searchRequest.query,searchRequest.caseSensitive?Qt::CaseSensitive:Qt::CaseInsensitive))
				return true;
		}
		break;
	default:
		return false;
	};
	return false;
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
	if(propertyContainer_->getPropertyValue("Name").toString().isEmpty())
	{
		QString errMsg = QString("Asset can not have an empty name.");
		addError("UIEnabled", errMsg,xmlStreamReader);
	}
	return true;
}

}//namespace Picto