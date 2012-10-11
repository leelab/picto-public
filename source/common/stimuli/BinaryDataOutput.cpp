#include "BinaryDataOutput.h"
#include "../memleakdetect.h"

namespace Picto {

const QString BinaryDataOutput::type = "Binary Data Output";

BinaryDataOutput::BinaryDataOutput()
: OutputSignal()
{
	AddDefinableProperty(QVariant::Int,"Value",0);
}

QSharedPointer<Asset> BinaryDataOutput::Create()
{
	return QSharedPointer<Asset>(new BinaryDataOutput());
}

void BinaryDataOutput::postDeserialize()
{
	OutputSignal::postDeserialize();
}

bool BinaryDataOutput::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!OutputSignal::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
