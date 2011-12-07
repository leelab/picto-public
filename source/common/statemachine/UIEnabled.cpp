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

QList<QSharedPointer<Transition>> UIEnabled::getDescendantsTransitions()
{
	QList<QSharedPointer<Transition>> descendantTrans;
	QList<QSharedPointer<Asset>> descendantAssets = getGeneratedChildren("Transition");
	while(descendantAssets.size())
	{
		descendantTrans.append(descendantAssets.takeFirst().staticCast<Transition>());
	}

	QStringList childTags = getDefinedChildTags();
	QList<QSharedPointer<Asset>> childList;
	foreach(QString childTag,childTags)
	{
		childList = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> child,childList)
		{
			if(child->inherits("Picto::UIEnabled"))
			{
				descendantTrans.append(child.staticCast<UIEnabled>()->getDescendantsTransitions());
			}
		}
	}
	return descendantTrans;
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