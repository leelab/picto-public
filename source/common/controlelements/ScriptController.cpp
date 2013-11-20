#include "ScriptController.h"
#include "../StateMachine/MachineContainer.h"
#include "../statemachine/LogicResult.h"
#include "../storage/ObsoleteAsset.h"

namespace Picto
{
ScriptController::ScriptController()
{
	currResult_ = "";
	AddDefinableObjectFactory("Type",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
	AddDefinableProperty(QVariant::String,"Script","");

	//This will allow designers to create their own results
	defineResultFactoryType("",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LogicResult::Create))));
}

/*! \brief Creates and returns a pointer to a new ScriptController object.
*/
ControlElement* ScriptController::NewScriptController()
{
	return new ScriptController;
}

/*! \brief Creates and returns a shared pointer to a new ScriptController object.
*/
QSharedPointer<Asset> ScriptController::Create()
{
	return QSharedPointer<Asset>(new ScriptController());
}

/*! \brief Returns the name of this type of ControlElement.  Namely, "Script Controller".
*/
QString ScriptController::ControllerType()
{
	return "Script Controller";
}

void ScriptController::start(QSharedPointer<Engine::PictoEngine>)
{
	currResult_.clear();
	isDone_ = false;
}

bool ScriptController::isDone(QSharedPointer<Engine::PictoEngine> engine)
{
	Q_UNUSED(engine);
	Property::startMonitoringForValueChange();
	QScriptValue returnVal(false);
	runScript(getName().simplified().remove(' '),returnVal);
	QString resultString = returnVal.toString();
	if(!returnVal.isUndefined() && ControlElement::getResult(resultString))
	{
		isDone_ = true;
		currResult_ = resultString;
		return true;
	}
	else
	{
		return false;
	}
}

QSharedPointer<Result> ScriptController::getResult()
{
	return ResultContainer::getResult(currResult_);
}
void ScriptController::upgradeVersion(QString deserializedVersion)
{
	ControlElement::upgradeVersion(deserializedVersion);
	if(deserializedVersion < "0.0.1")
	{	//Before 0.0.1, Script controllers had a single required result, "Done" that was
		//triggered if the script returned a boolean true value.  As of Syntax version 0.0.1,
		//the user can set as many results as they want with the result chosen by matching the
		//script's return string to the result name.  To upgrade, we add a "Done" result.  This
		//will cause this object's parent to hook up any saved "Done" result transitions to that
		//result.  We then change the name of the "Done" result to "true" which will cause
		//those transitions to update automatically.  This way when true is returned from these
		//script controllers, it will be cast to the string "true" and that result will be triggered, 
		//and the correct transition traversed.
		QList<QSharedPointer<Asset>> resultKids = getGeneratedChildren("Result");
		Q_ASSERT(resultKids.size() <=  1);
		if(resultKids.isEmpty())
		{	//If no "Done" result was deserialized, add it.
			addRequiredResult("Done");
			resultKids = getGeneratedChildren("Result");
		}
		Q_ASSERT(resultKids.size() ==  1);
		//Change the name of the "Done" result to "true"
		resultKids.first().staticCast<UIEnabled>()->setName("true");


		//Before 0.0.1, newClick on operatorClick info had the effect of reading out the current
		//"clicked" value, and also resetting it.  In 0.0.1, we changed it such that the user
		//must reset it manually.  Since there aren't too many experiments yet, we know how and
		//where this script was used and we are changing operatorClick.newClick to 
		//(operatorClick.newClick && !(operatorClick.newClick = false)) in all places that it was
		//used.
		QString frameScript = propertyContainer_->getPropertyValue("Script").toString();
		frameScript.replace("operatorClick.newClick","(operatorClick.newClick && !(operatorClick.newClick = false))");
		propertyContainer_->setPropertyValue("Script",frameScript);


	}
}

QString ScriptController::getReturnValueError(QString scriptName,const QScriptValue& returnValue)
{
	QString resultError = "";
	if(Property::valueWasChanged())
		resultError = QString("The Script in Script Controller: \"%1\" caused the property value: \"%2\" to change.\n"
			"The Script Controller Script should be used to select the result where contol flow needs to continue.\n"
			"Scripts that affect the experimental state should only be in Entry, Exit, or Frame Scripts.")
			.arg(getName())
			.arg(Property::changedValueName());
	if(resultError.isEmpty())
	{
		resultError = ControlElement::getReturnValueError(scriptName,returnValue);
	}
	return resultError;
}

void ScriptController::postDeserialize()
{
	ControlElement::postDeserialize();
	propertyContainer_->getProperty("UIEnabled")->setVisible(false);
}

bool ScriptController::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlElement::validateObject(xmlStreamReader))
		return false;
	return true;
}


bool ScriptController::hasScripts()
{
	if(ControlElement::hasScripts())
		return true;
	return (propertyContainer_->getPropertyValue("Script").toString() != "");
}

QMap<QString,QString>  ScriptController::getScripts()
{
	QMap<QString,QString>  scripts = ControlElement::getScripts();
	if(!propertyContainer_->getPropertyValue("Script").toString().isEmpty())
		scripts[getName().simplified().remove(' ')] = QString("Script");
	return scripts;
}

} //namespace Picto