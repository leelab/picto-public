#include "DigitalOutput.h"
#include "../memleakdetect.h"

namespace Picto {

const QString DigitalOutput::type = "Digital Output";

DigitalOutput::DigitalOutput()
: OutputSignal()
{
	AddDefinableProperty(QVariant::Int,"Pin",0);
	AddDefinableProperty(QVariant::Bool,"Value",false);
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
	if(getPin() < 0)
	{
		addError("Pin number cannot be less than zero");
		return false;
	}
	return true;
}

}; //namespace Picto
