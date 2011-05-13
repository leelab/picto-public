#include "ControlElement.h"

namespace Picto {

ControlElement::ControlElement()
{
	propertyContainer_ = PropertyContainer::create("ControlElement");
	propertyContainer_->addProperty(QVariant::String,"Name","");
	propertyContainer_->addProperty(QVariant::String,"Type","");

}


}; //namespace Picto
