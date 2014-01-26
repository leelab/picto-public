#include <QDebug>

#include "SwitchElement.h"
#include "../storage/AssetFactory.h"
#include "LogicResult.h"
#include "../memleakdetect.h"

namespace Picto
{
/*! \brief Constructs a new SwitchElement.
 *	\details Adds a Script Property that defines which Result control flow will continue from.  Sets up the ResultContainer ancestor
 *	class so that the object can have multiple LogicResult objects which will be triggerable from the Script logic.
 */
SwitchElement::SwitchElement()
{
	AddDefinableProperty(QVariant::String,"Script","");
	
	defineResultFactoryType("",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LogicResult::Create))));
}

/*! \brief Sets the content of this SwitchElement's Script Property to the input value.
*/
void SwitchElement::setScript(QString script)
{
	propertyContainer_->setPropertyValue("Script",script);
}

/*! \brief Creates a new SwitchElement object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> SwitchElement::Create()
{
	return QSharedPointer<Asset>(new SwitchElement());
}

/*! \brief Runs this SwitchElement's execution logic within the framework of the input PictoEngine.
 *	\details This function more or less performs the following run procedure:
 *		- Run EntryScript
 *		- Tell the Property class to start checking for any Property value changes.
 *		- Run the Script contained in the Script Property.
 *		- getReturnValueError() is automatically called as part of Script execution to verify that no Property values were changed by the Script and trigger a 
 *			runtime error if they were.
 *		- Picks out a Result based on the String returned from the Script.
 *		- Runs that Result's EntryScript.
 *		- Runs this SwitchElement's ExitScript.
 *		- Returns the name of the triggered Result.
 *	When an Analysis is active during the live experiment (in the TestViewer), AnalysisScripts are run before and after the Entry and Exit Scripts.
*/
QString SwitchElement::run(QSharedPointer<Engine::PictoEngine> engine)
{
	resetScriptableValues();
	runEntryScript();
	Property::startMonitoringForValueChange();
	QScriptValue returnVal = QScriptValue(false);
	runScript(getName().simplified().remove(' '),returnVal);
	QString resultString = returnVal.toString();
	//Switch Elements no longer have exit scripts
	//runExitScript();
	QSharedPointer<Result> result = getResult(resultString);
	if(result.isNull())
	{
		//If the string didn't match a result, the getReturnValueError function will return an error which
		//will come up in the debugger.  As far as this function goes, just go to the first result in the result list.
		Q_ASSERT(getResultList().length());
		result = getResult(getResultList().first());
	}
	setLatestResult(result->getName());
	result->runResultScript();
	runExitScript();
	return result->getName();
}

/*! \brief When a SwitchElement is run as a slave, it really doesn't do anything since the SlaveExperimentDriver handles
 *	changing of Property values and transitioning control flow based on what happened in the Master experiment.  
 *	This function therefore does nothing and should probably be removed.
 *	\sa StateMachineElement::slaveRun()
 */
QString SwitchElement::slaveRun(QSharedPointer<Engine::PictoEngine> engine)
{
	QString result;
	return result; 
}

/*! \brief Implements StateMachineElement::getReturnValueError() to verify that the SwitchElement's Script didn't
 *	cause any Property values to change and trigger a runtime error if it did.
 *	\details We do this to make sure that the SwitchElement Script's purpose is entirely
 *	to manage control flow branching and not to change any StateMachineElement data.  This keeps the StateMachine code
 *	much cleaner and easier to read because it means that all logic that affects the StateMachine's data is located in
 *	Entry, Exit or Frame Scripts.
*/
QString SwitchElement::getReturnValueError(QString scriptName,const QScriptValue& returnValue)
{
	if(scriptName != getName().simplified().remove(' '))
		return "";

	QString resultError = "";
	if(Property::valueWasChanged())
		resultError = QString("The Script in Switch Element: \"%1\" caused the property value: \"%2\" to change.\n"
			"The Switch element Script should be used to select the result where contol flow needs to continue.\n"
			"Scripts that affect the experimental state should only be in Entry, Exit, or Frame Scripts.")
			.arg(getName())
			.arg(Property::changedValueName());

	if(getResult(returnValue.toString()).isNull())
		resultError = QString("The Script in Switch Element: \"%1\" returned a value: \"%2\" that was not in its results list.\n")
		.arg(getName())
		.arg(returnValue.toString());
	if(resultError.isEmpty())
	{
		resultError = StateMachineElement::getReturnValueError(scriptName,returnValue);
	}
	return resultError;
}

void SwitchElement::postDeserialize()
{
	StateMachineElement::postDeserialize();
	propertyContainer_->getProperty("UIEnabled")->setVisible(false);
}

/*! \brief Extends StateMachineElement::validateObject() to verify that the SwitchElement's Script is not empty.*/
bool SwitchElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!StateMachineElement::validateObject(xmlStreamReader))
		return false;
	if(propertyContainer_->getPropertyValue("Script").toString().isEmpty())
	{
		addError("A Switch Element's switching script cannot be empty.");
		return false;
	}
	return true;
}

bool SwitchElement::hasScripts()
{
	if(StateMachineElement::hasScripts())
		return true;
	return (propertyContainer_->getPropertyValue("Script").toString() != "");
}

QMap<QString,QString>  SwitchElement::getScripts()
{
	QMap<QString,QString>  scripts = StateMachineElement::getScripts();
	if(!propertyContainer_->getPropertyValue("Script").toString().isEmpty())
		scripts[getName().simplified().remove(' ')] = QString("Script");
	return scripts;
}


} //namespace Picto