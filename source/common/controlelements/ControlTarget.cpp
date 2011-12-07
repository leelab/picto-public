#include <QPainter>
#include "ControlTarget.h"

namespace Picto
{

ControlTarget::ControlTarget(QPoint position, QColor color)
:
VisualElement(position,color)
{
	setActive(false);
	//These are redefined from Visual Element to fix the default values.
	AddDefinableProperty(QVariant::Bool,"OperatorView",true);
	AddDefinableProperty(QVariant::Bool,"SubjectView",false);
}

void ControlTarget::setActive(bool active)
{
	active_ = active;
}

void ControlTarget::postDeserialize()
{
	VisualElement::postDeserialize();
}


bool ControlTarget::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto