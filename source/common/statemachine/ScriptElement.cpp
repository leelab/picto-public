#include <QDebug>

#include "result.h"
#include "ScriptElement.h"
#include "../storage/SerializableFactory.h"

namespace Picto
{

ScriptElement::ScriptElement()
{
	AddDefinableProperty("Name","");
	AddDefinableProperty("Type","ScriptElement");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty(QVariant::String,"Script","");
	
	//At some point, we may want to make the default result name user modifiable...
	QSharedPointer<Result> r(new Result());
	r->setName("done");
	addResult(r);
}

void ScriptElement::setScript(QString script)
{
	propertyContainer_->setPropertyValue("Script",script);
}

/*! \brief Sets up the ScriptElement so it is ready to run scripts
 *	
 *	Setting up the element involves two steps:
 *		1. Adding our script as a function to the passed in engine
 *		2. Storing a pointer to the passed in engine
 */
bool ScriptElement::initScripting(QScriptEngine &qsEngine)
{
	//We allow ScriptElement names to have whitespace, so we need 
	//to get rid of it for a script function name
	QString functionName = getName().simplified().remove(' ');

	//confirm that this function doesn't already exist in the engine
	//If you hit this assert, it means that there are two ScriptElements
	//with the asme name.  The GUI shouldn't allow this...
	if(qsEngine.globalObject().property(functionName).isValid())
	{
		return false;
	}

	QString script = propertyContainer_->getPropertyValue("Script").toString();
	QString function = "function " + functionName + "() { " + script + "}";

	//add the function to the engine by calling evaluate on it
	qsEngine.evaluate(function);
	if(qsEngine.hasUncaughtException())
	{
		QString errorMsg = "Uncaught exception in Script Element " + getName() +"\n";
		errorMsg += QString("Line %1: %2\n").arg(qsEngine.uncaughtExceptionLineNumber())
										  .arg(qsEngine.uncaughtException().toString());
		errorMsg += QString("Backtrace: %1\n").arg(qsEngine.uncaughtExceptionBacktrace().join(", "));
		qDebug()<<errorMsg;
	}

	//Store a pointer to the engine
	qsEngine_ = &qsEngine;

	return true;
}

QString ScriptElement::run(QSharedPointer<Engine::PictoEngine> engine)
{
	QString functionName = getName().simplified().remove(' ');
	
	qsEngine_->globalObject().property(functionName).call();
	
	if(qsEngine_->hasUncaughtException())
	{
		QString errorMsg = "Uncaught exception in Script Element " + getName() +"\n";
		errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
										  .arg(qsEngine_->uncaughtException().toString());
		errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
		qDebug()<<errorMsg;
	}

	return "done";
}

QString ScriptElement::runAsSlave(QSharedPointer<Engine::PictoEngine> engine)
{
	//This will execute the script
	run(engine);

	QString result;
	while(result.isEmpty())
	{
		result = getMasterStateResult(engine);
		QCoreApplication::processEvents();
	}

	return result; 
}

bool ScriptElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}

/*!	\brief Converts the ScriptElement to an XML fragment
 *
 *	The XML fragment will look something like this:
 *		<StateMachineElement type = "ScriptElement">
 *			<Name>IncrementTrial</Name>
 *			<Script>Trial.setValue(Trial.getValue() + 1);</Script>
 *		</StateMachineElement>
 */
////


} //namespace Picto