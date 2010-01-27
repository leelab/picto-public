#include "StringParameter.h"

namespace Picto {

StringParameter::StringParameter()
{
	type_ = "String";
}

void StringParameter::addOption(StringParameterOption option)
{
	options_.append(option);
}

}; //namespace Picto
