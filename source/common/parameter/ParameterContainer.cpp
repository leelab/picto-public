#include "ParameterContainer.h"

namespace Picto {

ParameterContainer::ParameterContainer()
{
}

void ParameterContainer::addParameter(QSharedPointer<Parameter> parameter)
{
	parameters_.append(parameter);
}

bool ParameterContainer::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	//TODO: implement me

	return false;
}

bool ParameterContainer::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//TODO: implement me

	return false;
}

}; //namespace Picto
