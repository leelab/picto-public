#ifndef _SCRIPTABLECONTAINER_H_
#define _SCRIPTABLECONTAINER_H_

#include <QSharedPointer>
#include <QList>
#include <QScriptEngine>
#include <QScriptProgram>
#include <QScriptEngineDebugger>

#include "../common.h"
#include "../statemachine/UIEnabled.h"
#include "../property/Property.h"

#include "Scriptable.h"

namespace Picto {

/*! \brief A base class for all classes that need to be able to call scripts on in-scope Scriptable objects.
 *	\details This class handles all of the logic necessary to define which Scriptable objects are in scope, add them to a
 *	scripting engine, call scripts within that scripting engine and trigger runtime errors when there are problems with a
 *	called script.
 *
 *	Classes that want to run scripts should inherit this class and implement canHaveScripts(), hasScripts(), and
 *	getScripts().  They may find it useful to implement scriptableContainerWasReinitialized() so that they will have a
 *	way to know when something in the script environment changed, and getReturnValueError() to trigger runtime errors
 *	from C++.
 *
 *	\note Since our scoping system is slightly different from the Javascript scoping system, we handle scoping by
 *	creating a separate script engine for every ScriptableContainer and adding all in-scope Scriptable objects into it.  
 *	This means that in a given Experiment, there might be hundreds of script engines, which obviously takes up memory and 
 *	is just generally inefficient.  It also means that when multiple runtime errors occur, different debugging consoles
 *	open up in the TestViewer depending on which ScriptableContainer contained the offending script, which is pretty
 *	annoying.  There may be a way to better handle scoping issues by using QScriptContext or something like that, but I
 *	didn't have time to look into it.  This would be a good area for future research.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API ScriptableContainer : public Scriptable
#else
class ScriptableContainer : public Scriptable
#endif
{
	Q_OBJECT
public:
	ScriptableContainer();
	virtual ~ScriptableContainer(){};

	void addScriptable(QWeakPointer<Scriptable> scriptable);
	void addChildScriptableContainer(QSharedPointer<ScriptableContainer> child);
	QList<QWeakPointer<Scriptable>> getScriptableList();

	/*!	\brief Initializes scripting for this and all child ScriptableContainers
	 *	@param enableDebugging Enables debugging for this object.  Debugging will be triggered if there is a script
	 *	exception or a "debugger;" line is input into the script.
	 *	\note Returns false if there was an initialization error.
	 */
	bool initScripting(bool enableDebugging);
	/*! \brief Calls reset on the scriptables serialized into this container.
	 */
	void resetScriptableValues();
	void resetScriptableAnalysisValues();

	virtual QString friendlyTypeName(){return "Scriptable Container";};
	virtual QString getInfo();
	virtual void ClearAssociateChildren(QUuid associateId);

	bool deepReinitScripting(bool enableDebugging);

protected:
	void runScript(QString scriptName);
	void runScript(QString scriptName, QScriptValue& scriptReturnVal);
	virtual QString getReturnValueError(QString scriptName,const QScriptValue& returnValue);
	virtual QString defaultTagName(){return "Scriptables";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	/*! \brief Returns true if this ScriptableContainer can potentially include scripts that would need access to the
	 *	scripting engine.
	 */
	virtual bool canHaveScripts(){return false;};
	/*! \brief Returns true if this ScriptableContainer has scripts that need access to the scripting engine.
	 *	\note If this function returns true, canHaveScripts() should also return true.
	 */
	virtual bool hasScripts(){return false;};
	/*! \brief Returns a lookup table containing the names of Properties containing scripts indexed by the name by which
	 *	those scripts will be accessible.
	 *	\note There is some history here.  Originally, these scripts were stored as part of the scripting engine, and so
	 *	their names had to be unique.  Now they are stored in a table of QScriptProgram objects and so there is probably
	 *	not any real need for their "script names" to be different from the "Property names" of the Properties containing
	 *	the script text.  We should probably change this at some point.
	 */
	virtual QMap<QString,QString> getScripts(){return QMap<QString,QString> ();};
	/*! \brief Called when the ScriptableContainer is reinitialized.  Descendant classes extend this to run any code that
	 *	needs to have access to all Scriptable objects that will be accessible at runtime.
	 *	\sa initScripting()
	 */
	virtual void scriptableContainerWasReinitialized(){};
	virtual bool executeSearchAlgorithm(SearchRequest searchRequest);

	void deepDeinitScripting();

	/*! \brief Contains a shared pointer to the QScriptEngine
	 */
	QSharedPointer<QScriptEngine> qsEngine_;

	/*! \brief Contains a shared pointer to the QScriptEngineDebugger
	*/
	QSharedPointer<QScriptEngineDebugger> qsEngineDebugger_;

private:
	bool bindScriptablesToScriptEngine(QScriptEngine &engine);
	/*!	\brief This has to be weak because it will contain pointers to its parents.
	 */
	QList<QWeakPointer<Scriptable> > scriptables_;	
	/*!	\brief This holds scritables that are in scope but can't be bound (ie. Analysis Scriptables for an Experimental
	 *	Script Container).  It has to be weak because it will contain pointers to its parents.
	 */
	QList<QWeakPointer<Scriptable> > unboundScriptables_;	
	QList<QSharedPointer<ScriptableContainer> > scriptableContainers_;
	QMap<QString,QString>  scriptableListProperties_;
	QHash<QString,QSharedPointer<QScriptProgram>> scriptPrograms_;
	bool scriptingInitialized_;
	bool debuggingEnabled_;
	bool isAssociateElement_;

private slots:
	void deinitScripting();
	void deinitScripting(Property* prop,QVariant value);
	void addChildToScriptLists(QSharedPointer<Asset> newChild);

private:
	static QScriptValue checkScriptResults(QScriptContext *context, QScriptEngine *engine);
	static ScriptableContainer* objectForResultCheck_;
	static QScriptValue checkScriptResultScript_;

	friend class ScriptableContainer;
};

}; //namespace Picto

#endif
