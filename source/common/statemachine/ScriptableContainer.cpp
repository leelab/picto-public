#include <QMap>
#include <QPair>
#include <QDebug>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QAction>
#include <QTextEdit>
#include <QCoreApplication>
#include <QScriptSyntaxCheckResult>

#include "ScriptableContainer.h"
#include "../property/EnumProperty.h"

#include "../parameter/AssociateElement.h"
#include "../memleakdetect.h"

namespace Picto
{

ScriptableContainer* ScriptableContainer::objectForResultCheck_ = NULL;
QScriptValue ScriptableContainer::checkScriptResultScript_;

ScriptableContainer::ScriptableContainer()
	:
	scriptingInitialized_(false),
	debuggingEnabled_(false)
{
}

/*! \brief Adds a Scriptable object to this ScriptableContainer and makes it accessible to all
	*	ScriptableContainer objects for which it will be in scope.
	*	\details This function adds the Scriptable to a list of contained scriptables_, recursively calls
	*	addScriptable() on child ScriptableContainers to add it to them, connects to various signals
	*	from the input Scriptable so that this object will know when something about a child Scriptable
	*	changes, then calls deinitScripting() so that the new Scriptable will be correctly added to the
	*	scripting engine when the time comes for setting it up.
	*	\note If the input Scriptable is an AssociateElement, this ScriptableContainer takes care to
	*	only add it to its own contained scriptables_ list if this ScriptableContainer is also an
	*	AssociateElement with the same AssocaiteId.  This way, Analysis Scriptable objects
	*	will only be accessible to scripts in the same Analysis, but experimental Scriptable objects
	*	will be accessible to all ScriptableContainers, experimental or Analysis.
	*	\sa AnalysisScriptHolder
	*/
void ScriptableContainer::addScriptable(QWeakPointer<Scriptable> scriptable)
{
	//If the scriptable is empty (ie. Whatever the weak pointer was pointing to has been deleted) return
	if (scriptable.isNull())
		return;
	QSharedPointer<Scriptable> strScriptable = scriptable.toStrongRef();
	QSharedPointer<AssociateElement> assocElement = strScriptable.dynamicCast<AssociateElement>();
	if (assocElement)
	{
		//The input scriptable is part of an Associate.  Only add it if its part of the same Associate
		//as me
		if (!isAssociateElement_)
			unboundScriptables_.push_back(scriptable);
		else
		{
			QUuid scriptableAssocId = assocElement->getAssociateId();
			AssociateElement* thisAsAssocElem = dynamic_cast<AssociateElement*>(this);
			if (thisAsAssocElem->getAssociateId() != scriptableAssocId)
			{
				unboundScriptables_.push_back(scriptable);
			}
			else
				scriptables_.push_back(scriptable);
		}
	}
	else
		scriptables_.push_back(scriptable);

	//If we added a new scriptable, scripting is no longer properly initialized.
	scriptingInitialized_ = false;
	//If the new scriptable's name was edited, we'll need to reinitialize scripting again.
	connect(scriptable.toStrongRef().data(), SIGNAL(nameEdited()), this, SLOT(deinitScripting()));
	foreach(QSharedPointer<ScriptableContainer> child, scriptableContainers_)
	{
		child->addScriptable(scriptable);
	}
}

/*! \brief Adds a child ScriptableContainer to this scriptable container so that this object
	*	can add its child Scriptable objects to that ScriptableContainer and thereby make them
	*	available for scripts in that container.
	*	\details This ScriptableContainer adds both experimental and Analysis Scriptable objects
	*	to its child ScriptableContainers in case so that any Analysis ScriptableContainer descendants
	*	will be able to access appropriate in-scope Analysis Scriptables.
	*	\sa AnalysisScriptHolder
	*/
void ScriptableContainer::addChildScriptableContainer(QSharedPointer<ScriptableContainer> child)
{
	scriptableContainers_.push_back(child);
	//Add all of my scriptables to the child
	foreach(QWeakPointer<Scriptable> scriptable, scriptables_)
	{
		child->addScriptable(scriptable);
	}
	//Try to add unbound scriptables to the child too
	foreach(QWeakPointer<Scriptable> scriptable, unboundScriptables_)
	{
		child->addScriptable(scriptable);
	}

	if (scriptingInitialized_)
	{	//Our engine and all child ScriptableContainers engines are initialized, so 
		//initialize this one too.
		child->initScripting(debuggingEnabled_);
	}
}

/*! \brief Returns a list of all Scriptable objects available to scripts in this ScriptableContainer.
*/
QList<QWeakPointer<Scriptable>> ScriptableContainer::getScriptableList()
{
	return scriptables_;
}

/*! \brief Initializes the script environment by creating a script engine and adding all child
	*	Scriptable objects (scriptables_) to it.
	*	\details This function also sets up all QScriptProgram objects for this objects scripts so that
	*	they can be used to operate on contained Scriptable objects.  By using the scriptingInitialized_ flag
	*	we can assure that the actual initialization only occurs if something changed about this object that
	*	requires everything to be reinitialized.  This function also creates a QScriptEngineDebugger object and
	*	attaches it to the engine when debugging is enabled by setting the enableDebugging input to true.  This
	*	causes a debugging console to appear whenever a scripting runtime error occurs.
	*
	*	\note Sometimes C++ code needs to have the capability to trigger a runtime error.  This is true in the
	*	case of Analysis scripts that have access to experimental Scriptable objects but are not allowed
	*	to change their values, only read them, and if they do change a value, a runtime error is triggered.
	*	To support this case, when debugging is enabled and after every script that is called, checkScriptResults()
	*	is called with the result of that script which eventually ends up calling the virtual getReturnValueError()
	*	function to check for an error.  This system is set up in this function.
	*	\sa checkScriptResults(), getReturnValueError()
	*/
bool ScriptableContainer::initScripting(bool enableDebugging)
{
	if (scriptingInitialized_)
		return true;

	debuggingEnabled_ = enableDebugging;
	//Initialize scripting on this ScriptableContainer
	if (canHaveScripts())
	{
		if (hasScripts())
		{
			//Looks like we're going to need a script engine
			//create the engine
			qsEngine_ = QSharedPointer<QScriptEngine>(new QScriptEngine());

			if (debuggingEnabled_)
			{
				//Attach the debugger to the script engine
				qsEngineDebugger_ = QSharedPointer<QScriptEngineDebugger>(new QScriptEngineDebugger());
				qsEngineDebugger_->attachTo(qsEngine_.data());

				//Add result check function to qsEngine
				qsEngine_->globalObject().setProperty("checkScriptResults", qsEngine_->newFunction(checkScriptResults));
			}


			//Bind all scriptable to the script engine (parameters and ui elements)
			if (!bindScriptablesToScriptEngine(*qsEngine_))
				return false;

			QMap<QString, QString>  scriptMap = getScripts();
			scriptPrograms_.clear();
			for (QMap<QString, QString>::iterator it = scriptMap.begin(); it != scriptMap.end(); it++)
			{
				//if(qsEngine_->globalObject().property(it.key()).isValid())
				//{
				//	// There is already a scriptable component with this name.
				//	Q_ASSERT(false);
				//	return false;
				//}

				//Make a Qt Script Function out of the script and its name
				QString scriptTag = it.value();
				QString script = propertyContainer_->getPropertyValue(scriptTag).toString();
				Q_ASSERT(script.length());
				Q_ASSERT(!scriptPrograms_.contains(it.key()));
				QString programText;
				if (!debuggingEnabled_)
				{
					//Add a version of the script with return value checking.
					programText = QString("function script(){\n\n//YOUR CODE STARTS HERE-------\n%1\n//YOUR CODE ENDS HERE----------\n\n}\nscript()").arg(script);
				}
				else
				{
					programText = QString("function script(){\n\n//YOUR CODE STARTS HERE-------\n%1\n//YOUR CODE ENDS HERE----------\n\n}\nvar returnVal = script();\n").arg(script) + QString("checkScriptResults(\"%1\",returnVal);\nreturnVal;").arg(it.key());
				}
				scriptPrograms_[it.key()] = QSharedPointer<QScriptProgram>(new QScriptProgram(programText));
			}
		}
		//Connect this object's edited slot to deinitScripting, because if one of its scripts changes, we'll need to reinsert
		//it into the scripting environment
		//Calling disconnect first assures us that we don't connect the signal more than once and end up calling deinitScripting
		//more than once when edited() is emitted.
		disconnect(this, SIGNAL(edited()), this, SLOT(deinitScripting()));
		connect(this, SIGNAL(edited()), this, SLOT(deinitScripting()));
	}

	//Inform child classes that the scriptable container has been reinitialized.
	scriptableContainerWasReinitialized();

	//Intialize Scripting on all child ScriptableContainers
	foreach(QSharedPointer<ScriptableContainer> child, scriptableContainers_)
	{
		child->initScripting(debuggingEnabled_);
	}
	scriptingInitialized_ = true;
	return true;
}

/*! \brief Causes all contained Scriptable objects to have their enteredScope() function called.
	*	\details This function should probably be called performScopeEntryOperations() or something like
	*	that.  Its probably worth changing it.  It is called by descendant class objects whenever
	*	they enter scope so that, among other things, all of their child Scriptable objects' Property
	*	values will be reset to their initValues.
	*	\note This function internally calls resetScriptableAnalysisValues() as well.
	*	\sa Scriptable::enteredScope()
	*/
void ScriptableContainer::resetScriptableValues()
{
	QStringList childTags = getDefinedChildTags();
	foreach(QString childTag, childTags)
	{
		QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> tagChild, tagChildren)
		{
			if (tagChild->inherits("Picto::Scriptable"))
			{
				tagChild.staticCast<Scriptable>()->enteredScope();
			}
		}
	}

	//Reset values on active analysis elements
	resetScriptableAnalysisValues();
}

/*! \brief Causes all contained Analysis Scriptable objects that are currently active to have their enteredScope() function called.
	*	\details This function should probably be called performAnalysisScopeEntryOperations() or something like
	*	that.  Its probably worth changing it.  It is called by resetScriptableValues() so that, among other things,
	*	all of this object's active child Analysis Scriptable objects' Property values will be reset to their initValues.
	*	\sa resetScriptableValues(), Scriptable::enteredScope()
	*/
void ScriptableContainer::resetScriptableAnalysisValues()
{
	QStringList childTags;
	//Reset values on active analysis elements
	QList<QUuid> activeAnalysisIds = getDesignConfig()->getActiveAnalysisIds();
	foreach(QUuid analysisId, activeAnalysisIds)
	{
		childTags = getAssociateChildTags(analysisId);
		foreach(QString childTag, childTags)
		{
			QList<QSharedPointer<Asset>> tagChildren = getAssociateChildren(analysisId, childTag);
			foreach(QSharedPointer<Asset> tagChild, tagChildren)
			{
				if (tagChild->inherits("Picto::Scriptable"))
				{
					tagChild.staticCast<Scriptable>()->enteredScope();
				}
			}
		}
	}
}

/*! \brief Returns a formatted string containing all in-scope script properties and script function for this ScriptableContainer.
	*	\details This essentially just calls Scriptable::getScriptingInfo() on all contained Scriptable objects
	*	and puts them together in a nice formatted string.
	*	\note This was created before AssetDescriber and since that class is so much more useful, we haven't been maintaining
	*	this function and its underlying documentation system to seriously.  You will notice for example, that only
	*	experimental Scriptable objects are included even if this element is an AssociateElement.
	*	\sa Scriptable::getScriptingInfo(), AssetDescriber
	*/
QString ScriptableContainer::getInfo()
{
	QString returnVal = Scriptable::getInfo();
	QString scriptMemberStr;
	if (canHaveScripts())
	{
		foreach(QSharedPointer<Scriptable> scriptable, scriptables_)
		{
			scriptMemberStr.append(scriptable->getScriptingInfo());
		}
		if (returnVal.length())
		{
			scriptMemberStr.prepend("<h4>Available Scriptable Members</h4>");
		}
		else
			scriptMemberStr.append("<h4>No Scriptable Members Available</h4>");
		returnVal.append(scriptMemberStr);
	}
	return returnVal;
}

/*! \brief Extends Scriptable::ClearAssociateChildren() to remove all Scriptable objects from this objects
	*	list of contained Scriptable objects and ScriptableContainer objects.
	*/
void ScriptableContainer::ClearAssociateChildren(QUuid associateId)
{
	Scriptable::ClearAssociateChildren(associateId);

	//Go through the scriptables_ and unboundScriptables_ lists and remove all associate children with the input associate id.
	bool somethingWasRemoved = false;
	AssociateElement* associateElem;
	for (QList<QWeakPointer<Scriptable>>::iterator iter = scriptables_.begin(); iter != scriptables_.end();)
	{
		Q_ASSERT(!iter->isNull());
		associateElem = dynamic_cast<AssociateElement*>(iter->data());
		if (!associateElem)
		{
			iter++;
			continue;
		}
		if (associateElem->getAssociateId() == associateId)
		{
			iter = scriptables_.erase(iter);
			somethingWasRemoved = true;
			continue;
		}
		iter++;
	}

	for (QList<QWeakPointer<Scriptable>>::iterator iter = unboundScriptables_.begin(); iter != unboundScriptables_.end();)
	{
		Q_ASSERT(!iter->isNull());
		associateElem = dynamic_cast<AssociateElement*>(iter->data());
		if (!associateElem)
		{
			iter++;
			continue;
		}
		if (associateElem->getAssociateId() == associateId)
		{
			iter = unboundScriptables_.erase(iter);
			somethingWasRemoved = true;
			continue;
		}
		iter++;
	}

	for (QList<QSharedPointer<ScriptableContainer>>::iterator iter = scriptableContainers_.begin(); iter != scriptableContainers_.end();)
	{
		Q_ASSERT(!iter->isNull());
		associateElem = dynamic_cast<AssociateElement*>(iter->data());
		if (!associateElem)
		{
			iter++;
			continue;
		}
		if (associateElem->getAssociateId() == associateId)
		{
			iter = scriptableContainers_.erase(iter);
			somethingWasRemoved = true;
			continue;
		}
		iter++;
	}

	if (somethingWasRemoved)
	{
		//If we removed a scriptable, scripting is no longer properly initialized.
		scriptingInitialized_ = false;
	}
}

/*! \brief Used instead of runScript(QString,QScriptValue &) when no return value is needed.
	*/
void ScriptableContainer::runScript(QString scriptName)
{
	QScriptValue returnVal;
	runScript(scriptName, returnVal);
}

/*! \brief Runs the script with the input scriptName, setting the result of the script into the scriptReturnVal reference.
	*	\details The scriptName here is the script name returned from getScripts().
	*	\note This function takes part in the system allowing C++ code to trigger script runtime errors by telling the
	*	static checkScriptResults() function that return values should be checked using this object's getReturnValueError()
	*	function.
	*	\sa checkScriptResults(), initScripting()
	*/
void ScriptableContainer::runScript(QString scriptName, QScriptValue& scriptReturnVal)
{
	initScripting(debuggingEnabled_);	//Make sure this scriptable container has scripting initialized before attempting to run one of its scripts
	Q_ASSERT(qsEngine_);
	objectForResultCheck_ = this;
	Q_ASSERT(scriptPrograms_.contains(scriptName));
	if (scriptPrograms_.contains(scriptName))
		scriptReturnVal = qsEngine_->evaluate(*scriptPrograms_.value(scriptName));
	objectForResultCheck_ = NULL;
}

/*! \brief This function returns an error string if the input returnValue is invalid or there
	*	was some other problem with the latest script that was run.
	*	\details The QString sciptName entry is the name of the script that was just run as returned
	*	by getScripts().  This function is called by checkScriptResults() which will trigger
	*	a runtime error with the returned string as an error message if the returned string is not
	*	empty.
	*	\sa checkScriptResults(), initScripting()
	*/
QString ScriptableContainer::getReturnValueError(QString, const QScriptValue&)
{
	return QString();
}

void ScriptableContainer::postDeserialize()
{
	Scriptable::postDeserialize();
	isAssociateElement_ = dynamic_cast<AssociateElement*>(this);
	scriptPrograms_.clear();
	QStringList childTags = getDefinedChildTags();
	foreach(QString childTag, childTags)
	{
		QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> tagChild, tagChildren)
		{
			addChildToScriptLists(tagChild);
		}
	}
	connect(this, SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)), this, SLOT(addChildToScriptLists(QSharedPointer<Asset>)));
}

/*! \brief Extends Scriptable::validateObject() to check the syntax of this object's scripts and add a compilation
	*	error if any problems are found.
	*/
bool ScriptableContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if (!Scriptable::validateObject(xmlStreamReader))
		return false;

	//Check Script Syntax
	QMap<QString, QString> scriptMap = getScripts();
	bool scriptsAreOkay = true;
	for (QMap<QString, QString>::iterator it = scriptMap.begin(); it != scriptMap.end(); it++)
	{
		QString scriptTag = it.value();
		QString script = propertyContainer_->getPropertyValue(scriptTag).toString();
		QString function = "function " + it.key() + "() { " + script + "\n}";
		QScriptSyntaxCheckResult res = QScriptEngine::checkSyntax(function);
		if (res.state() != QScriptSyntaxCheckResult::Valid)
		{
			QString errorMessage = QString("Syntax Error in \"%1\"(Line:%2)")
				.arg(scriptTag)
				.arg(res.errorLineNumber())
				.arg(res.errorMessage());
			if (res.errorMessage().size())
				errorMessage.append(" : " + res.errorMessage());
			addError(errorMessage);
			scriptsAreOkay = false;
		}
		if (!scriptsAreOkay)
			return false;
	}
	//We don't check if multiple scriptables have the same name, because our bindScriptablesToScriptEngine()
	//function makes sure that only the one with the longest path is added to the scriptEngine
	return true;
}

/*! \brief Extends Scriptable::executeSearchAlgorithm to check for the input searchRequest in this object's
	*	scripts.
	*/
bool ScriptableContainer::executeSearchAlgorithm(SearchRequest searchRequest)
{
	if (Scriptable::executeSearchAlgorithm(searchRequest))
		return true;
	switch (searchRequest.type)
	{
	case SearchRequest::SCRIPT:
	{
		//Check to see if this object contains any non-empty scripts
		QMap<QString, QString>  scriptMap = getScripts();
		QSharedPointer<Property> currScriptProp;
		QString currScript;
		foreach(QString key, scriptMap.keys())
		{
			QString propName = scriptMap.value(key);
			currScriptProp = propertyContainer_->getProperty(propName);
			if (currScriptProp.isNull())
				continue;
			if (!currScriptProp->value().toString().isEmpty())
				return true;
		}
	}
	break;
	case SearchRequest::STRING:
	{
		//Search through my scripts to see if any contain the query string
		QMap<QString, QString>  scriptMap = getScripts();
		QSharedPointer<Property> currScriptProp;
		QString currScript;
		foreach(QString key, scriptMap.keys())
		{
			QString propName = scriptMap.value(key);
			//Search script contents
			currScriptProp = propertyContainer_->getProperty(propName);
			if (currScriptProp.isNull())
				continue;
			if (currScriptProp->value().toString().contains(searchRequest.query, searchRequest.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive))
				return true;
		}
	}
	break;
	default:
		return false;
	};
	return false;
}

/*! \brief Called by initScripting() to bind all of this objects contained Scriptable objects to the input
	*	script engine so that they can be accessed by this object's scripts.
	*	\details To maintain a logical scoping system, when two in scope Scriptable objects have the same name
	*	only the one with the longer path (ie. the one that is closer to this ScriptableContainer in the design
	*	tree) will be added to the engine.
	*/
bool ScriptableContainer::bindScriptablesToScriptEngine(QScriptEngine &engine)
{
	//If any scriptables have the same name, the one with the shorter path
	//should not be bound.  Create a list with all bindable scriptables.
	QHash<QString, QSharedPointer<Scriptable>> scriptableHash;
	QString currName;
	foreach(QSharedPointer<Scriptable> scriptable, scriptables_)
	{
		currName = scriptable->getName();
		if (!scriptableHash.contains(currName))
		{
			scriptableHash[currName] = scriptable;
		}
		else
		{
			if (scriptableHash.value(currName)->getPath().size() < scriptable->getPath().size())
				scriptableHash[currName] = scriptable;
		}
	}
	QList<QSharedPointer<Scriptable>> bindableScriptables = scriptableHash.values();
	foreach(QSharedPointer<Scriptable> scriptable, bindableScriptables)
	{
		if (!scriptable->bindToScriptEngine(engine))
			return false;
	}
	return true;
}
/*! \brief Called if something about a scriptable changes, so that the script
	*	engine will be reinitialized before use.
	*	\details If a Scriptable object's name changes for example, we need to reinitialize
	*	scripting so that the object is accessible in the script engine under the correct name.
	*/
void ScriptableContainer::deinitScripting()
{
	scriptingInitialized_ = false;
}

/*! \brief Equivalent to deinitScripting() with a different signature so that it can be attached to signals with the same inputs.
	*	\note This appears to no longer be used.  We should probably get rid of it.
	*/
void ScriptableContainer::deinitScripting(Property*, QVariant)
{
	deinitScripting();
}

/*! \brief Adds the input newChild Asset to this objects Scriptable or ScriptableContainer
	*	lists if it is of either of those types.
	*/
void ScriptableContainer::addChildToScriptLists(QSharedPointer<Asset> newChild)
{
	if (newChild.dynamicCast<Scriptable>())
		addScriptable(newChild.staticCast<Scriptable>());
	if (newChild.dynamicCast<ScriptableContainer>())
		addChildScriptableContainer(newChild.staticCast<ScriptableContainer>());
}

/*! \brief When debugging is enabled, this function is used to call a ScriptableContainer objects
	*	getReturnValueError() function and trigger a runtime error if there was some problem with the
	*	latest script that was called.
	*	\details Qt requires us to use a QScriptContext object in order to trigger a runtime
	*	error manually.  Since we sometimes want to trigger runtime errors from C++, when
	*	debugging is enabled we tell the script engine to call this function with the results
	*	of the latest script just after it is called.  Since the runScript() function makes
	*	sure to store which object just called a script, this function can then call its getReturnValueError()
	*	function to find out if there was any problem with the script call, and if so trigger a runtime error.
	*	This may seem like a complicated way to do something trivial, but it is simply the way that Qt forces us
	*	to do it.
	*	\sa  getReturnValueError(), initScripting()
	*/
QScriptValue ScriptableContainer::checkScriptResults(QScriptContext *context, QScriptEngine*)
{
	QScriptValue returnVal;
	if (!objectForResultCheck_)
		return returnVal;
	if (context->argumentCount() != 2)
		return returnVal;
	QString resultError = objectForResultCheck_->getReturnValueError(context->argument(0).toString(), context->argument(1));
	if (resultError.isEmpty())
		return returnVal;
	context->throwError(resultError);
	return returnVal;
}
}; //namespace Picto
