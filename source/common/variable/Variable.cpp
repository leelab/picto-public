#include "Variable.h"
#include "../memleakdetect.h"

namespace Picto {

Variable::Variable()
{
}

void Variable::postDeserialize()
{
	Scriptable::postDeserialize();
	propertyContainer_->getProperty("UIEnabled")->setVisible(false);
}

}; //namespace Picto
