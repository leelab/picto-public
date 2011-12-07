#include "ParameterFactory.h"
#include "../memleakdetect.h"

namespace Picto
{

QMap<QString, ParameterFactory::NewParameterFnPtr> ParameterFactory::parameters_;

ParameterFactory::ParameterFactory()
{
}

QSharedPointer<Parameter> ParameterFactory::generateParameter(QString type)
{
	if(!parameters_.contains(type))
		return QSharedPointer<Parameter>();

	QSharedPointer<Parameter> p(parameters_[type]());
	return p;
}

void ParameterFactory::addParameterType(QString type, NewParameterFnPtr newFnPtr)
{
	if(parameters_.contains(type))
		return;

	parameters_[type] = newFnPtr;
}

} //namespace Picto
