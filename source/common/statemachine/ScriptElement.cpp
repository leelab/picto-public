#include <QDebug>

#include "result.h"
#include "ScriptElement.h"
#include "../storage/DataStoreFactory.h"

namespace Picto
{

ScriptElement::ScriptElement()
{
	AddProperty(propertyContainer_->setPropertyValue("Name",""));
	AddProperty(propertyContainer_->setPropertyValue("Type","ScriptElement"));
	AddProperty(propertyContainer_->addProperty(QVariant::String,"Script",""));

	//QSharedPointer<DataStoreFactory> factory(new DataStoreFactory(propertyContainer_->getPropertyValue
	
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

/*!	\brief Converts the ScriptElement to an XML fragment
 *
 *	The XML fragment will look something like this:
 *		<StateMachineElement type = "ScriptElement">
 *			<Name>IncrementTrial</Name>
 *			<Script>Trial.setValue(Trial.getValue() + 1);</Script>
 *		</StateMachineElement>
 */
//bool ScriptElement::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("StateMachineElement");
//	xmlStreamWriter->writeAttribute("type","ScriptElement");
//
//	xmlStreamWriter->writeTextElement("Name", propertyContainer_->getPropertyValue("Name").toString());
//
//	QString script = propertyContainer_->getPropertyValue("Script").toString();
//	xmlStreamWriter->writeTextElement("Script",script);
//	return true;
//}

//bool ScriptElement::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateMachineElement")
//	{
//		addError("ScriptElement","Incorrect tag, expected <StateMachineElement>",xmlStreamReader);
//		return false;
//	}
//	if(xmlStreamReader->attributes().value("type").toString() != type())
//	{
//		addError("ScriptElement","Incorrect type of StateMachineElement, expected ScriptElement",xmlStreamReader);
//		return false;
//	}
//
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateMachineElement") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->name().toString();
//		if(name == "Name")
//		{
//			propertyContainer_->setPropertyValue("Name",QVariant(xmlStreamReader->readElementText()));
//		}
//		else if(name == "Script")
//		{
//			QString script = xmlStreamReader->readElementText();
//			propertyContainer_->setPropertyValue("Script",script);
//		}
//		else
//		{
//			addError("ScriptElement", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//		xmlStreamReader->readNext();
//	}
//	
//	if(xmlStreamReader->atEnd())
//	{
//		addError("ScriptElement", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//
//	return true;
//}


} //namespace Picto