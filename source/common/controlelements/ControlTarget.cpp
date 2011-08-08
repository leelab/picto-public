#include "ControlTarget.h"
#include <QPainter>
namespace Picto
{

ControlTarget::ControlTarget(QPoint position, QColor color)
:
VisualElement(position,color)
{
	setActive(false);
	AddDefinableProperty(QVariant::Bool,"OperatorVisible",true);
	AddDefinableProperty(QVariant::Bool,"SubjectVisible",false);
}

void ControlTarget::setActive(bool active)
{
	active_ = active;
}

void ControlTarget::postSerialize()
{
	VisualElement::postSerialize();
}


bool ControlTarget::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto