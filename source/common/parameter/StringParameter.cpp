#include "StringParameter.h"
#include "../memleakdetect.h"

namespace Picto {

StringParameter::StringParameter()
: Parameter()
{
	AddDefinableProperty(QVariant::String,"Value",QVariant());
}

Parameter* StringParameter::NewParameter()
{
	//This parameter isn't finished yet
	Q_ASSERT(false);
	return NULL;
}

QSharedPointer<Asset> StringParameter::Create()
{
	return QSharedPointer<Asset>(new StringParameter());
}

//void StringParameter::setValue(QVariant value)
//{
//	//StringParameters haven't been built yet...
//	Q_ASSERT(false);
//}
//
//QVariant StringParameter::getValue()
//{
//	//StringParameters haven't been built yet...
//	Q_ASSERT(false);
//	return QVariant();
//}


void StringParameter::addOption(StringParameterOption option)
{
	options_.append(option);
}

//bool StringParameter::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	return false;
//}
//bool StringParameter::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	addError("StringParameter", "Deserialization function not yet implemented", xmlStreamReader);
//	return false;
//}

void StringParameter::postDeserialize()
{
	Parameter::postDeserialize();
	setPropertyRuntimeEditable("Value");
}

bool StringParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	addError("StringParameter", "StringParameter is not yet implemented", xmlStreamReader);
	return true;
}

}; //namespace Picto
