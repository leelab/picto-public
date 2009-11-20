#include "Experiment.h"

namespace Picto {

Experiment::Experiment() :
	formatID_("1.0.0.0"),
	parameterContainer_("Experiment")
{
	parameterContainer_.addParameter(Parameter(QVariant::String,"Name","Unnamed Experiment"));
}

void Experiment::addTask(Picto::Task * //task
						 )
{
}

}; //namespace Picto
