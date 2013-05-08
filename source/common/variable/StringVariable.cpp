#include "StringVariable.h"
#include "../memleakdetect.h"

namespace Picto {

StringVariable::StringVariable()
: Variable()
{
	AddDefinableProperty(QVariant::String,"Value",QVariant());
}

QSharedPointer<Asset> StringVariable::Create()
{
	return QSharedPointer<Asset>(new StringVariable());
}


void StringVariable::postDeserialize()
{
	Variable::postDeserialize();
}

bool StringVariable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Variable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
