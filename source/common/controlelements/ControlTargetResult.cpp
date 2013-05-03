#include "ControlTargetResult.h"
namespace Picto
{

ControlTargetResult::ControlTargetResult()
{
	AddDefinableProperty("ControlTarget","");
}

QSharedPointer<Asset> ControlTargetResult::Create()
{
	return QSharedPointer<Asset>(new ControlTargetResult());
}

bool ControlTargetResult::contains(int x, int y)
{
	if(controlTarget_.isNull())
		return false;
	return controlTarget_.toStrongRef()->contains(x,y);
}

void ControlTargetResult::setActive(bool active)
{
	if(controlTarget_.isNull())
		return;
	controlTarget_.toStrongRef()->setActive(active);
}

void ControlTargetResult::postDeserialize()
{
	Result::postDeserialize();
}


bool ControlTargetResult::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//! \todo Add script verification once validate runs after full deserialization
	if(!Result::validateObject(xmlStreamReader))
		return false;
	return true;
}

void ControlTargetResult::scriptableContainerWasReinitialized()
{
	Result::scriptableContainerWasReinitialized();
	QList<QWeakPointer<Scriptable>> scriptables = getScriptableList();
	QString targetName = propertyContainer_->getPropertyValue("ControlTarget").toString();
	foreach(QWeakPointer<Scriptable> scriptable,scriptables)
	{
		if(scriptable.isNull())
			continue;
		if(scriptable.toStrongRef()->getName() == targetName && scriptable.toStrongRef()->inherits("Picto::ControlTarget"))
		{
			controlTarget_ = scriptable.toStrongRef().staticCast<ControlTarget>();
			break;
		}
	}
}

}//namespace Picto