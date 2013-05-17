#include "UIElement.h"
#include "../memleakdetect.h"
namespace Picto
{

UIElement::UIElement()
{
	AddDefinableProperty(QVariant::Point,"Pos",QPoint());
	AddDefinableProperty(QVariant::String,"Notes","");
	EXP_LINK_FACTORY_CREATION
}

QSharedPointer<Asset> UIElement::Create()
{
	return QSharedPointer<Asset>(new UIElement());
}

void UIElement::setPos(QPoint pos)
{
	propertyContainer_->setPropertyValue("Pos",pos);
}

QPoint UIElement::getPos()
{
	return propertyContainer_->getPropertyValue("Pos").toPoint();
}

void UIElement::setNotes(QString notes)
{
	propertyContainer_->setPropertyValue("Notes",notes);
}

QString UIElement::getNotes()
{
	return propertyContainer_->getPropertyValue("Notes").toString();
}

void UIElement::postDeserialize()
{
	DataStore::postDeserialize();
}

bool UIElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!DataStore::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto