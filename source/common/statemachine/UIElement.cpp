#include "UIElement.h"
#include "../memleakdetect.h"
namespace Picto
{

/*! \brief Creates a new UIElement.
 *	\details Adds a "Pos" position property and a "Notes" property.
 */
UIElement::UIElement()
{
	AddDefinableProperty(QVariant::Point,"Pos",QPoint());
	AddDefinableProperty(QVariant::String,"Notes","");
	EXP_LINK_FACTORY_CREATION
}

/*! \brief Creates a new UIElement and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> UIElement::Create()
{
	return QSharedPointer<Asset>(new UIElement());
}

/*! \brief Sets the position of the linked Asset in the Designer canvas.*/
void UIElement::setPos(QPoint pos)
{
	propertyContainer_->setPropertyValue("Pos",pos);
}
/*! \brief Gets the position of the linked Asset in the Designer canvas.*/
QPoint UIElement::getPos()
{
	return propertyContainer_->getPropertyValue("Pos").toPoint();
}
/*! \brief Sets the notes for the linked Asset in the Designer canvas.*/
void UIElement::setNotes(QString notes)
{
	propertyContainer_->setPropertyValue("Notes",notes);
}
/*! \brief Gets the notes for the linked Asset in the Designer canvas.*/
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