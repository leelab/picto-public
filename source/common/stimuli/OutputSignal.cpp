#include <QScriptValue>

#include "OutputSignal.h"
#include "../memleakdetect.h"

namespace Picto {

OutputSignal::OutputSignal()
{
	AddDefinableProperty(QVariant::String,"Port","BNC0");
	AddDefinableProperty(QVariant::Bool,"Enabled",true);
}

OutputSignal::~OutputSignal()
{
}

void OutputSignal::postDeserialize()
{
	OutputElement::postDeserialize();
}

bool OutputSignal::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!OutputElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

