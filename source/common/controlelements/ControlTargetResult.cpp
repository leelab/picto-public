#include "ControlTargetResult.h"
namespace Picto
{

ControlTargetResult::ControlTargetResult()
{
	AddDefinableProperty("ControlTarget","");
}
/*! \brief Creates and returns a shared pointer to a ControlTargetResult object*/
QSharedPointer<Asset> ControlTargetResult::Create()
{
	return QSharedPointer<Asset>(new ControlTargetResult());
}
/*! \brief A convenience function that calls this object's underlying ControlTarget objects ControlTarget::contains() function.
 *	\sa ControlTarget::contains()
 */
bool ControlTargetResult::contains(int x, int y)
{
	if(controlTarget_.isNull())
		return false;
	return controlTarget_.toStrongRef()->contains(x,y);
}
/*! \brief A convenience function that calls this object's underlying ControlTarget objects ControlTarget::setActive() function.
 *	\sa ControlTarget::setActive()
 */
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
	// \todo Add script verification once validate runs after full deserialization
	if(!Result::validateObject(xmlStreamReader))
		return false;
	if(propertyContainer_->getPropertyValue("ControlTarget").toString().isEmpty())
	{
		addError("Control Target Results cannot have an empty 'ControlTarget' property.");
		return false;
	}
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

bool ControlTargetResult::executeSearchAlgorithm(SearchRequest searchRequest)
{
	if(LogicResult::executeSearchAlgorithm(searchRequest))
		return true;
	switch(searchRequest.type)
	{
	case SearchRequest::STRING:
		{
			//Search my control target for the string
			QString controlTargetName = propertyContainer_->getPropertyValue("ControlTarget").toString();
			if(!controlTargetName.isNull() && controlTargetName.contains(searchRequest.query,searchRequest.caseSensitive?Qt::CaseSensitive:Qt::CaseInsensitive))
				return true;
		}
		break;
	};
	return false;
}
}//namespace Picto