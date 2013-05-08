#include "NumberVariable.h"
#include "../memleakdetect.h"

namespace Picto {

NumberVariable::NumberVariable()
: Variable()
{
	AddDefinableProperty(QVariant::Double,"Value",QVariant());
}

QSharedPointer<Asset> NumberVariable::Create()
{
	return QSharedPointer<Asset>(new NumberVariable());
}


void NumberVariable::postDeserialize()
{
	Variable::postDeserialize();
}

bool NumberVariable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Variable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
