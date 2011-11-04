#include "ControlResult.h"
namespace Picto
{

ControlResult::ControlResult()
{
	AddDefinableProperty("ControlTarget","");
}

bool ControlResult::contains(int x, int y)
{
	if(controlTarget_.isNull())
		return false;
	return controlTarget_->contains(x,y);
}

void ControlResult::setActive(bool active)
{
	if(controlTarget_.isNull())
		return;
	controlTarget_->setActive(active);
}

void ControlResult::postDeserialize()
{
	Result::postDeserialize();
}


bool ControlResult::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//! \todo Add script verification once validate runs after full deserialization
	if(!Result::validateObject(xmlStreamReader))
		return false;
	return true;
}

void ControlResult::scriptableContainerWasReinitialized()
{
	Result::scriptableContainerWasReinitialized();
	QList<QSharedPointer<Scriptable>> scriptables = getScriptableList();
	QString targetName = propertyContainer_->getPropertyValue("ControlTarget").toString();
	foreach(QSharedPointer<Scriptable> scriptable,scriptables)
	{
		if(scriptable->getName() == targetName && scriptable->inherits("Picto::ControlTarget"))
		{
			controlTarget_ = scriptable.staticCast<ControlTarget>();
			break;
		}
	}
}

}//namespace Picto