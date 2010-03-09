#include <QDebug>

#include "result.h"
#include "ScriptElement.h"

namespace Picto
{

ScriptElement::ScriptElement()
{
	propertyContainer_.setPropertyValue("Type","ScriptElement");
	propertyContainer_.addProperty(Property(QVariant::String,"Script",""));
	
	//At some point, we may want to make the default result name user modifiable...
	QSharedPointer<Result> r(new Result());
	r->setName("done");
	addResult(r);
}

void ScriptElement::setScript(QString script)
{
	propertyContainer_.setPropertyValue("Script",script);
}


QString ScriptElement::run()
{
	parameterContainer_.addAsScriptProperties(qsEngine_);
	QString script = propertyContainer_.getPropertyValue("Script").toString();
	qsEngine_.evaluate(script);
	
	if(qsEngine_.hasUncaughtException())
	{
		QString errorMsg = "Uncaught exception in Script Element" + getName() +"\n";
		errorMsg += QString("Line %1: %2\n").arg(qsEngine_.uncaughtExceptionLineNumber())
										  .arg(qsEngine_.uncaughtException().toString());
		errorMsg += QString("Backtrace: %1\n").arg(qsEngine_.uncaughtExceptionBacktrace().join(", "));
		qDebug()<<errorMsg;
	}

	return "done";
}

/*!	\brief Converts the ScriptElement to an XML fragment
 *
 *	The XML fragment will look something like this:
 *		<StateMachineElement type = "ScriptElement">
 *			<Name>IncrementTrial</Name>
 *			<Script>Trial.setValue(Trial.getValue() + 1);</Script>
 *		</StateMachineElement>
 */
bool ScriptElement::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("StateMachineElement");
	xmlStreamWriter->writeAttribute("type","ScriptElement");

	xmlStreamWriter->writeTextElement("Name", propertyContainer_.getPropertyValue("Name").toString());

	QString script = propertyContainer_.getPropertyValue("Script").toString();
	xmlStreamWriter->writeTextElement("Script",script);
	return true;
}

bool ScriptElement::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateMachineElement")
	{
		addError("ScriptElement","Incorrect tag, expected <StateMachineElement>",xmlStreamReader);
		return false;
	}
	if(xmlStreamReader->attributes().value("type").toString() != type())
	{
		addError("ScriptElement","Incorrect type of StateMachineElement, expected ScriptElement",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateMachineElement") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Name")
		{
			propertyContainer_.setPropertyValue("Name",QVariant(xmlStreamReader->readElementText()));
		}
		else if(name == "Script")
		{
			QString script = xmlStreamReader->readElementText();
			propertyContainer_.setPropertyValue("Script",script);
		}
		else
		{
			addError("ScriptElement", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}
	
	if(xmlStreamReader->atEnd())
	{
		addError("ScriptElement", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	return true;
}


} //namespace Picto