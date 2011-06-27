#include "ScriptableContainer.h"
#include <QMap>
#include <QDebug>
#include <QScriptValueIterator>

using namespace Picto;

ScriptableContainer::ScriptableContainer()
: scriptableFactory_(new AssetFactory(0,-1)),
  scriptingInitialized_(false)
{
	AddDefinableObjectFactory("Scriptable",scriptableFactory_);
}

/*! \brief Copies the scriptables from the passed in container
 *	
 *	This is used when adding an child element to a parent scriptableContainer.  By passing
 *	in the parent scriptable container, we now have access to all 
 *	of the parent's scriptables.  Effectively this allows us to
 *	have scriptables with global scope.
 */
void ScriptableContainer::addScriptables(QSharedPointer<ScriptableContainer> scriptableContainer)
{
	addScriptables(scriptableContainer.data());
}

void ScriptableContainer::addScriptables(ScriptableContainer *scriptableContainer)
{
	QList<QSharedPointer<Scriptable>> scriptables = scriptableContainer->getScriptableList();
	foreach(QSharedPointer<Scriptable> scriptable,scriptables)
	{
		addScriptable(scriptable);
	}
}

void ScriptableContainer::addScriptable(QSharedPointer<Scriptable> scriptable)
{
	scriptables_.push_back(scriptable);
	//If we added a new scriptable, scripting is no longer properly initialized.
	scriptingInitialized_ = false;
	//If the new scriptable's name edited, we'll need to reinitialize scripting again.
	connect(scriptable.data(),SIGNAL(nameEdited()),this,SLOT(deinitScripting()));
	foreach(QSharedPointer<ScriptableContainer> child,scriptableContainers_)
	{
		child->addScriptable(scriptable);
	}
}

void ScriptableContainer::addChildScriptableContainer(QSharedPointer<ScriptableContainer> child)
{
	scriptableContainers_.push_back(child);
	child->addScriptables(this);
	if(scriptingInitialized_)
	{	//Our engine and all child ScriptableContainers engines are initialized, so 
		//initialize this one too.
		child->initScripting();
	}
}

//QSharedPointer<Scriptable> ScriptableContainer::getScriptable(QString name)
//{
//	foreach(QSharedPointer<Scriptable> scriptable,scriptables_)
//		if(scriptable->getName() == name)
//			return scriptable;
//	return QSharedPointer<Scriptable>();
//}

//! \brief Returns a list of all contained parameters
QList<QSharedPointer<Scriptable>> ScriptableContainer::getScriptableList()
{
	return scriptables_;
}

bool ScriptableContainer::initScripting(bool forDesign)
{
	if(	scriptingInitialized_ )
		return true;

	//Initialize scripting on this ScriptableContainer
	if(forDesign || hasScripts() )
	{
		//Looks like we're going to need a script engine
		//create the engine
		qsEngine_ = QSharedPointer<QScriptEngine>(new QScriptEngine());

		//Bind all scriptable to the script engine (parameters and ui elements)
		if(!bindToScriptEngine(*qsEngine_))
			return false;

		QMap<QString,QString> scriptMap = getScripts();
		for(QMap<QString,QString>::iterator it = scriptMap.begin();it!=scriptMap.end();it++)
		{
			if(qsEngine_->globalObject().property(it.key()).isValid())
			{
				// There is already a scriptable component with this name.
				return false;
			}

			//Make a Qt Script Function out of the script and its name
			QString function = "function " + it.key() + "() { " + it.value() + "}";

			//add the function to the engine by calling evaluate on it
			qsEngine_->evaluate(function);
			//Check for errors
			if(qsEngine_->hasUncaughtException())
			{
				QString errorMsg = "Uncaught exception in Script Element " + getName() +"\n";
				errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
												  .arg(qsEngine_->uncaughtException().toString());
				errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
				qDebug()<<errorMsg;
				return false;
			}
		}
	}

	//Intialize Scripting on all child ScriptableContainers
	foreach(QSharedPointer<ScriptableContainer> child,scriptableContainers_)
	{
		child->initScripting(forDesign);
	}
	scriptingInitialized_ = true;
	return true;
}

QString ScriptableContainer::getInfo()
{
	if(!initScripting(true))
		return "Script Parsing Error.";
	if(!qsEngine_)
		return "No Parameters Available";
	QString returnVal;
	QScriptValueIterator it(qsEngine_->globalObject());
	while (it.hasNext())
	{
		it.next();
		if(!(it.flags() & (QScriptValue::SkipInEnumeration | QScriptValue::Undeletable)))
		{
			returnVal.append(QString("%1\n").arg(it.name()));
			QScriptValueIterator it1(it.value());
			while(it1.hasNext())
			{
				it1.next();
				if(!(it1.flags() & (QScriptValue::SkipInEnumeration | QScriptValue::Undeletable)))
				{
					returnVal.append(QString("\t%1\n").arg(it1.name()));
				}
			}
		}
	}
	if(returnVal.length())
		returnVal.prepend("Available Members:\n");
	else
		return "No Parameters Available";
	return returnVal;
}

bool ScriptableContainer::bindToScriptEngine(QScriptEngine &engine)
{
	foreach(QSharedPointer<Scriptable> scriptable, scriptables_)
	{
		if(!scriptable->bindToScriptEngine(engine))
			return false;
	}
	return true;
}

void ScriptableContainer::runScript(QString scriptName)
{
	bool returnVal;
	runScript(scriptName,returnVal);
}

void ScriptableContainer::runScript(QString scriptName, bool& scriptReturnVal)
{
	scriptReturnVal = qsEngine_->globalObject().property(scriptName).call().toBool();
	if(qsEngine_->hasUncaughtException())
	{
		QString errorMsg = "Uncaught exception in State" + getName() +", script "+scriptName+"\n";
		errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
										  .arg(qsEngine_->uncaughtException().toString());
		errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
		qDebug()<<errorMsg;
	}
}

void ScriptableContainer::postSerialize()
{
	UIEnabled::postSerialize();
	QList<QSharedPointer<Asset>> newScriptables = getGeneratedChildren("Scriptable");
	foreach(QSharedPointer<Asset> newScriptable,newScriptables)
	{
		QString name = newScriptable->getName();
		addScriptable(newScriptable.staticCast<Scriptable>());
	}
}

bool ScriptableContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	QList<QSharedPointer<Asset>> newScriptables = getGeneratedChildren("Scriptable");
	QList<QSharedPointer<Asset>>::iterator ita,itb;
	for(ita = newScriptables.begin();ita != newScriptables.end();ita++)
	{
		for(itb = ita+1;itb != newScriptables.end();itb++)
		{
			if((*itb)->getName() == (*ita)->getName())
			{
				addError("ScriptableContainer", QString("Muliple scriptables have the same name: \"%1\"").arg((*ita)->getName()).toAscii(), xmlStreamReader);
				return false;
			}
		}
	}
	return true;
}

void ScriptableContainer::deinitScripting()
{
	scriptingInitialized_ = false;
}
