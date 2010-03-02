#include "ControlElement.h"

namespace Picto {

ControlElement::ControlElement()
:propertyContainer_("ControlElement")
{
	propertyContainer_.addProperty(Property(QVariant::String,"Name",""));
	propertyContainer_.addProperty(Property(QVariant::String,"Type",""));

}




}; //namespace Picto
