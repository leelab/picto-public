#include "ControlElement.h"

namespace Picto {

ControlElement::ControlElement()
{
	//propertyContainer_->addProperty(QVariant::String,"Name","");
	//propertyContainer_->addProperty(QVariant::String,"Type","");

}

bool ControlElement::addResult(QString resultName)
{
	if(results_.contains(resultName))
		return false;
	else
	{
		results_.push_back(resultName);
		return true;
	}
}


}; //namespace Picto
