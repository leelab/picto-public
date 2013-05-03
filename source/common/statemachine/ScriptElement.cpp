#include <QDebug>

#include "ScriptElement.h"
#include "../storage/AssetFactory.h"
#include "../memleakdetect.h"

namespace Picto
{

ScriptElement::ScriptElement()
{
	setMaxOptionalResults(0,"");
	//AddDefinableProperty("Type","ScriptElement");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty(QVariant::String,"Script","");
	
	addRequiredResult("true");
	addRequiredResult("false");
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
//bool ScriptElement::initScripting(QScriptEngine &qsEngine)
//{
//	//We allow ScriptElement names to have whitespace, so we need 
//	//to get rid of it for a script function name
//	QString functionName = getName().simplified().remove(' ');
//
//	//confirm that this function doesn't already exist in the engine
//	//If you hit this assert, it means that there are two ScriptElements
//	//with the asme name.  The GUI shouldn't allow this...
//	if(qsEngine.globalObject().property(functionName).isValid())
//	{
//		return false;
//	}
//
//	QString script = propertyContainer_->getPropertyValue("Script").toString();
//	QString function = "function " + functionName + "() { " + script + "}";
//
//	//add the function to the engine by calling evaluate on it
//	qsEngine.evaluate(function);
//	if(qsEngine.hasUncaughtException())
//	{
//		QString errorMsg = "Uncaught exception in Script Element " + getName() +"\n";
//		errorMsg += QString("Line %1: %2\n").arg(qsEngine.uncaughtExceptionLineNumber())
//										  .arg(qsEngine.uncaughtException().toString());
//		errorMsg += QString("Backtrace: %1\n").arg(qsEngine.uncaughtExceptionBacktrace().join(", "));
//		qDebug()<<errorMsg;
//	}
//
//	//Store a pointer to the engine
//	qsEngine_ = &qsEngine;
//
//	return true;
//}

QSharedPointer<Asset> ScriptElement::Create()
{
	return QSharedPointer<Asset>(new ScriptElement());
}

QString ScriptElement::run(QSharedPointer<Engine::PictoEngine> engine)
{
	resetScriptableValues();
	QScriptValue returnVal = QScriptValue(false);
	runScript(getName().simplified().remove(' '),returnVal);
	if(returnVal.toBool())
		return "true";
	else
		return "false";
}

QString ScriptElement::slaveRun(QSharedPointer<Engine::PictoEngine> engine)
{
	QString result;
	return result; 
}

void ScriptElement::postDeserialize()
{
	StateMachineElement::postDeserialize();
}

bool ScriptElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!StateMachineElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

bool ScriptElement::hasScripts()
{
	if(StateMachineElement::hasScripts())
		return true;
	return (propertyContainer_->getPropertyValue("Script").toString() != "");
}

QMap<QString,QPair<QString,QString>>  ScriptElement::getScripts()
{
	QMap<QString,QPair<QString,QString>>  scripts = StateMachineElement::getScripts();
	if(!propertyContainer_->getPropertyValue("Script").toString().isEmpty())
		scripts[getName().simplified().remove(' ')] = QPair<QString,QString>(QString(),"Script");
	return scripts;
}


} //namespace Picto