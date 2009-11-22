#include "Task.h"

namespace Picto {

Task::Task() :
	propertyContainer_("Task")
{
	propertyContainer_.addProperty(Property(QVariant::String,"Name","Unnamed Task"));
}

}; //namespace Picto
