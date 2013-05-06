#include "Var.h"
#include "../memleakdetect.h"

namespace Picto {

Var::Var()
: Variable()
{
	AddDefinableProperty(QVariant::String,"Value",QVariant());
}

QSharedPointer<Asset> Var::Create()
{
	return QSharedPointer<Asset>(new Var());
}


void Var::postDeserialize()
{
	Variable::postDeserialize();
}

bool Var::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Variable::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
