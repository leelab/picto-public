#include "Task.h"

namespace Picto {

Task::Task() :
	parameterContainer_("Task")
{
	parameterContainer_.addParameter(Parameter(QVariant::String,"Name","Unnamed Task"));
}

}; //namespace Picto
