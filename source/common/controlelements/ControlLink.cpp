#include "ControlLink.h"

namespace Picto
{

ControlLink::ControlLink()
: Transition()
{
}

ControlLink::ControlLink(QString source, QString sourceResult, QString destination)
: Transition(source,sourceResult,destination)
{
}

}