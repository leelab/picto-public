#include "ControlTarget.h"
namespace Picto
{

ControlTarget::ControlTarget()
{
	AddDefinableProperty("Bounds",QRect());
}

QRect ControlTarget::getBounds()
{
	return propertyContainer_->getPropertyValue("Bounds").toRect();
}

void ControlTarget::postSerialize()
{
	Result::postSerialize();
}


bool ControlTarget::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Result::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto