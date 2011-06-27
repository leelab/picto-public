#include "Parameter.h"

namespace Picto {

Parameter::Parameter() :
	bOperatorUI_(false),
	type_("null"),
	order_(0)
{
}

QString Parameter::type()
{
	return type_;
}


}; //namespace Picto
