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
		QList<QSharedPointer<Asset>> resultChildren = getGeneratedChildren("Result");
		bool found = false;
		foreach(QSharedPointer<Asset> result,resultChildren)
		{
			if(result.staticCast<Property>()->value().toString() == resultName)
			{
				found = true;
				break;
			}
		}
		if(!found)
			AddChild("Result",propertyContainer_->addProperty(QVariant::String,"Result",resultName,true));
		return true;
	}
}


}; //namespace Picto
