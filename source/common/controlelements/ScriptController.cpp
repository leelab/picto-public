#include "ScriptController.h"

namespace Picto
{
ScriptController::ScriptController()
{
	
	AddDefinableProperty("Type","");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty(QVariant::String,"Script","");

	//Make sure to update the list of results...
	addRequiredResult("Done");
}


ControlElement* ScriptController::NewScriptController()
{
	return new ScriptController;
}

QSharedPointer<Asset> ScriptController::Create()
{
	return QSharedPointer<Asset>(new ScriptController());
}

QString ScriptController::ControllerType()
{
	return "Script Controller";
}

void ScriptController::start(QSharedPointer<Engine::PictoEngine>)
{
	isDone_ = false;
}

bool ScriptController::isDone(QSharedPointer<Engine::PictoEngine> engine)
{
	Q_UNUSED(engine);
	
	QScriptValue returnVal(false);
	runScript(getName().simplified().remove(' '),returnVal);
	if(returnVal.toBool())
	{
		isDone_ = true;
		return true;
	}
	else
	{
		return false;
	}
}

QString ScriptController::getResult()
{
	if(isDone_)
		return "Done";
	else
		return "";
}

void ScriptController::postDeserialize()
{
	ControlElement::postDeserialize();
}

bool ScriptController::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlElement::validateObject(xmlStreamReader))
		return false;
	return true;
}


bool ScriptController::hasScripts()
{
	return (propertyContainer_->getPropertyValue("Script").toString() != "");
}

QMap<QString,QPair<QString,QString>>  ScriptController::getScripts()
{
	QMap<QString,QPair<QString,QString>>  scripts;
	if(!hasScripts())
		return scripts;
	scripts[getName().simplified().remove(' ')] = QPair<QString,QString>(QString(),propertyContainer_->getPropertyValue("Script").toString());
	return scripts;
}

} //namespace Picto