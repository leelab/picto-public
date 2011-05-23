#include "StringParameter.h"

namespace Picto {

StringParameter::StringParameter()
{
	type_ = "String";
}

Parameter* StringParameter::NewParameter()
{
	//This parameter isn't finished yet
	Q_ASSERT(false);
	return NULL;
}

void StringParameter::setValue(QVariant value)
{
	//StringParameters haven't been built yet...
	Q_ASSERT(false);
}

QVariant StringParameter::getValue()
{
	//StringParameters haven't been built yet...
	Q_ASSERT(false);
	return QVariant();
}


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

bool StringParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	addError("StringParameter", "StringParameter is not yet implemented", xmlStreamReader);
	return true;
}

}; //namespace Picto
