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

void StringParameter::addOption(StringParameterOption option)
{
	options_.append(option);
}

bool StringParameter::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	return false;
}
bool StringParameter::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return false;
}

}; //namespace Picto
