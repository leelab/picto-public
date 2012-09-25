#include "DigitalOutput.h"
#include "../memleakdetect.h"

namespace Picto {

const QString DigitalOutput::type = "Digital Output";

DigitalOutput::DigitalOutput()
: OutputSignal()
{
	AddDefinableProperty(QVariant::Bool,"Output",false);
}

QSharedPointer<Asset> DigitalOutput::Create()
{
	return QSharedPointer<Asset>(new DigitalOutput());
}

void DigitalOutput::postDeserialize()
{
	OutputSignal::postDeserialize();
}

bool DigitalOutput::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!OutputSignal::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
