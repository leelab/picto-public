#include "ColorParameter.h"
#include "../memleakdetect.h"

namespace Picto {

ColorParameter::ColorParameter()
: Parameter()
{
	AddDefinableProperty(QVariant::Color,"Value",QVariant());
}

Parameter* ColorParameter::NewParameter()
{
	return new ColorParameter;
}

QSharedPointer<Asset> ColorParameter::Create()
{
	return QSharedPointer<Asset>(new ColorParameter());
}


void ColorParameter::postDeserialize()
{
	Parameter::postDeserialize();
	setPropertyRuntimeEditable("Value");
}

bool ColorParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
