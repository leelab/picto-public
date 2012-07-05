#include <QDebug>
#include "ScriptFunction.h"
#include "../memleakdetect.h"

using namespace Picto;

ScriptFunction::ScriptFunction()
{
	AddDefinableProperty(QVariant::String,"Inputs","");
	AddDefinableProperty(QVariant::String,"Script","");
}

QSharedPointer<Asset> ScriptFunction::Create()
{
	return QSharedPointer<Asset>(new ScriptFunction());
}


bool ScriptFunction::bindToScriptEngine(QScriptEngine &engine)
{
	bool returnVal = ScriptableContainer::bindToScriptEngine(engine);
	if(propertyContainer_->getPropertyValue("Script").toString().isEmpty())
		return returnVal;

	//Make a Qt Script Function in the global frame
	QString function = 
		"function " + getName() + "("+propertyContainer_->getPropertyValue("Inputs").toString()+") "
		+"{ " 
			+ propertyContainer_->getPropertyValue("Script").toString() 
		+ " }";

	//add the function to the engine by calling evaluate on it
	engine.evaluate(function);
	//Check for errors
	if(engine.hasUncaughtException())
	{
		QString errorMsg = "Uncaught exception in Script Element " + getName() +"\n";
		errorMsg += QString("Line %1: %2\n").arg(engine.uncaughtExceptionLineNumber())
										  .arg(engine.uncaughtException().toString());
		errorMsg += QString("Backtrace: %1\n").arg(engine.uncaughtExceptionBacktrace().join(", "));
		qDebug()<<errorMsg;
		return false;
	}
	return true;
}

QString ScriptFunction::getScriptingInfo()
{
	QString returnVal = ScriptableContainer::getScriptingInfo();
	if(propertyContainer_->getPropertyValue("Script").toString().isEmpty())
		return returnVal;
	returnVal.append("<dl><dt><span style=\"font-style:italic\">");
	QString signature = getName() + "("+propertyContainer_->getPropertyValue("Inputs").toString()+")";
	QStringList commaSplitSig = signature.split(",");
	QStringList::iterator paramTypeIter = commaSplitSig.begin();
	//Finish name italicization
	int openParenthLoc = (*paramTypeIter).indexOf("(");
	(*paramTypeIter).insert(openParenthLoc,"</span>");
	//Color function name blue and the rest grey
	openParenthLoc = (*paramTypeIter).indexOf("(");
	(*paramTypeIter).insert(openParenthLoc+1,"<span style=\"color:blue\">");
	(*paramTypeIter).append("</span>");
	while((paramTypeIter != commaSplitSig.end()))
	{
		if(paramTypeIter != commaSplitSig.begin())
			(*paramTypeIter) = QString("<span style=\"color:blue\">%1</span>").arg(*paramTypeIter);
		paramTypeIter++;
	}
	signature = commaSplitSig.join(",");
	//Move the last name in front of the final paranthesis (right now it will be func(int a,int b,int) c
	int parenthInd = signature.lastIndexOf(")");
	signature.remove(parenthInd,1);
	signature.append(")");

	returnVal.append(signature);

	//Close the definition list.
	returnVal.append("</dt></dl>");
	return returnVal;
}

void ScriptFunction::postDeserialize()
{
	ScriptableContainer::postDeserialize();
}

bool ScriptFunction::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptableContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}