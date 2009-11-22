#include "Experiment.h"

namespace Picto {

Experiment::Experiment() :
	formatID_("1.0.0.0"),
	propertyContainer_("Experiment")
{
	propertyContainer_.addProperty(Property(QVariant::String,"Name","Unnamed Experiment"));
}

void Experiment::addTask(Picto::Task * //task
						 )
{
}

}; //namespace Picto
