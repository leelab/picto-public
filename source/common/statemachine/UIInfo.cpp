#include "UIInfo.h"
namespace Picto
{

UIInfo::UIInfo()
{
	AddDefinableProperty(QVariant::Point,"Pos",QPoint());
	AddDefinableProperty(QVariant::Point,"UniqueID",QString());
}

void UIInfo::setPos(QPoint pos)
{
	propertyContainer_->setPropertyValue("Pos",pos);
}

QPoint UIInfo::getPos()
{
	return propertyContainer_->getPropertyValue("Pos").toPoint();
}

void UIInfo::postDeserialize()
{
	DataStore::postDeserialize();
	propertyContainer_->getProperty("UniqueID")->setDeleted();
}


bool UIInfo::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto