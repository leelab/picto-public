#include <QScriptValue>

#include "OutputSignal.h"
#include "../memleakdetect.h"

namespace Picto {

OutputSignal::OutputSignal()
{
	AddDefinableProperty(QVariant::Int,"Port",0);
	AddDefinableProperty(QVariant::Bool,"Enabled",true);
}

OutputSignal::~OutputSignal()
{
}

void OutputSignal::postDeserialize()
{
	Scriptable::postDeserialize();
}

bool OutputSignal::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Scriptable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto

