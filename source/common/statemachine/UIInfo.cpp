#include "UIInfo.h"
namespace Picto
{

UIInfo::UIInfo()
{
	AddDefinableProperty(QVariant::Point,"Pos",QPoint());
}

void UIInfo::setPos(QPoint pos)
{
	propertyContainer_->setPropertyValue("Pos",pos);
}

QPoint UIInfo::getPos()
{
	return propertyContainer_->getPropertyValue("Pos").toPoint();
}

void UIInfo::postSerialize()
{
	DataStore::postSerialize();
}


bool UIInfo::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto