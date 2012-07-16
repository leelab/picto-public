#include "UIInfo.h"
#include "../memleakdetect.h"
namespace Picto
{

UIInfo::UIInfo()
{
	AddDefinableProperty(QVariant::Point,"Pos",QPoint());
	AddDefinableProperty(QVariant::Int,"OpenDescendant",0);
}

QSharedPointer<Asset> UIInfo::Create()
{
	return QSharedPointer<Asset>(new UIInfo());
}

void UIInfo::setPos(QPoint pos)
{
	propertyContainer_->setPropertyValue("Pos",pos);
}

QPoint UIInfo::getPos()
{
	return propertyContainer_->getPropertyValue("Pos").toPoint();
}
void UIInfo::setOpenDescendant(int assetId)
{
	propertyContainer_->setPropertyValue("OpenDescendant",assetId);
}
int UIInfo::getOpenDescendant()
{
	return propertyContainer_->getPropertyValue("OpenDescendant").toInt();
}
void UIInfo::postDeserialize()
{
	DataStore::postDeserialize();
}


bool UIInfo::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto