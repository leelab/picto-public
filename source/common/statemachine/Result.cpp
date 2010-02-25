#include "Result.h"

namespace Picto
{

Result::Result()
{
	type_ = "Result";
}

QString Result::run()
{
	return resultValue_;
}
}//namespace Picto