#include "Scriptable.h"
#include <QDebug>

namespace Picto {

Scriptable::Scriptable()
{
	
}

bool Scriptable::bindToScriptEngine(QScriptEngine &engine)
{
	QScriptValue qsValue = engine.newQObject(static_cast<Scriptable*>(this)/*,QScriptEngine::QtOwnership,QScriptEngine::ExcludeSuperClassContents*/);
	engine.globalObject().setProperty(getName(),qsValue);
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

bool Scriptable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	if(getName().contains(" "))
	{
		addError("Scriptable", "The name of a scriptable element cannot contain whitespace",xmlStreamReader);
		return false;
	}
	return true;
}

}; //namespace Picto
