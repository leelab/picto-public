#include "ControlLink.h"

namespace Picto
{

ControlLink::ControlLink()
: Transition()
{
}

ControlLink::ControlLink(QSharedPointer<Asset> source, QSharedPointer<Asset> sourceResult, QSharedPointer<Asset> destination)
: Transition(source,sourceResult,destination)
{
}

QSharedPointer<Asset> ControlLink::Create()
{
	return QSharedPointer<Asset>(new ControlLink());
}
}