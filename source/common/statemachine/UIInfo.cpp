#include "UIInfo.h"
namespace Picto
{

UIInfo::UIInfo()
{
	AddDefinableProperty(QVariant::Point,"Pos",QPoint());
	AddDefinableProperty(QVariant::String,"UniqueID","");
}

void UIInfo::setPos(QPoint pos)
{
	propertyContainer_->setPropertyValue("Pos",pos);
}

QPoint UIInfo::getPos()
{
	return propertyContainer_->getPropertyValue("Pos").toPoint();
}

QUuid UIInfo::getUniqueId()
{
	 QUuid uuid(propertyContainer_->getPropertyValue("UniqueID").toString());
	 if(uuid.isNull())
	 {
		 uuid = QUuid::createUuid();
		 propertyContainer_->setPropertyValue("UniqueID",uuid.toString());
	 }
	 return uuid;
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