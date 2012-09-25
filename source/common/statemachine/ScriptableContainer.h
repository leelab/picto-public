#ifndef _SCRIPTABLECONTAINER_H_
#define _SCRIPTABLECONTAINER_H_

#include <QSharedPointer>
#include <QList>
#include <QScriptEngine>
#include <QScriptEngineDebugger>

#include "../common.h"
#include "../statemachine/UIEnabled.h"

#include "Scriptable.h"

namespace Picto {

/*! \brief A container for storing items that can be used in scripts
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

	void addScriptables(QSharedPointer<ScriptableContainer> scriptableContainer);
	void addScriptables(ScriptableContainer* scriptableContainer);
	void addScriptable(QWeakPointer<Scriptable> scriptable);
	void addChildScriptableContainer(QSharedPointer<ScriptableContainer> child);
	QList<QWeakPointer<Scriptable>> getScriptableList();

	/*! \brief Initializes scripting for this and all child ScriptableContainers
	 * @param enableDebugging Enables debugging for this object.  Debugging will be
	 * triggered if there is a script exception or a "debugger;" line is input into the script.
	 * returns false if there was an initialization error.
	 */
	bool initScripting(bool enableDebugging);
	/*! \brief Calls reset on the scriptables serialized into this container.
	 */
	void resetScriptableValues();

	virtual QString assetType(){return "ScriptableContainer";};
	virtual QString getInfo();

protected:
	void runScript(QString scriptName);
	void runScript(QString scriptName, QScriptValue& scriptReturnVal);
	virtual QString defaultTagName(){return "Scriptables";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return false;};
	virtual bool hasScripts(){return false;};
	//This returns a map of QMap<script name,script code>
	virtual QMap<QString,QPair<QString,QString>>  getScripts(){return QMap<QString,QPair<QString,QString>> ();};
	virtual void scriptableContainerWasReinitialized(){};
	QSharedPointer<AssetFactory> visualElementFactory_;
	QSharedPointer<AssetFactory> parameterFactory_;
	QSharedPointer<AssetFactory> controlTargetFactory_;
	QSharedPointer<AssetFactory> audioElementFactory_;
	QSharedPointer<AssetFactory> outputSignalFactory_;
	QSharedPointer<AssetFactory> scriptFunctionFactory_;
	QSharedPointer<QScriptEngine> qsEngine_;
	QSharedPointer<QScriptEngineDebugger> qsEngineDebugger_;

private:
	bool bindScriptablesToScriptEngine(QScriptEngine &engine);
	QList<QWeakPointer<Scriptable> > scriptables_;	//This has to be weak because it will contain pointers to its parents.
	QList<QSharedPointer<ScriptableContainer> > scriptableContainers_;
	QMap<QString,QString>  scriptableListProperties_;
	bool scriptingInitialized_;
	bool debuggingEnabled_;

private slots:
	//This is called if something about a scriptable changed, so that the script
	//engines will need to be reinitialized before use.
	void deinitScripting();
};


}; //namespace Picto

#endif
