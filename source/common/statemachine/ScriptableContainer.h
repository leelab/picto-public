#ifndef _SCRIPTABLECONTAINER_H_
#define _SCRIPTABLECONTAINER_H_

#include "../common.h"
#include "../statemachine/UIEnabled.h"

#include "Scriptable.h"

#include <QSharedPointer>
#include <QList>
#include <QScriptEngine>

namespace Picto {

/*! \brief A container for storing items that can be used in scripts
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API ScriptableContainer : public UIEnabled
#else
class ScriptableContainer : public UIEnabled
#endif
{
	Q_OBJECT
public:
	ScriptableContainer();
	void addScriptables(QSharedPointer<ScriptableContainer> scriptableContainer);
	void addScriptables(ScriptableContainer* scriptableContainer);
	void addScriptable(QSharedPointer<Scriptable> scriptable);
	void addChildScriptableContainer(QSharedPointer<ScriptableContainer> child);
	QList<QSharedPointer<Scriptable>> getScriptableList();

	/*! \brief Initializes scripting for this and all child ScriptableContainers
	 * If for design is true, all child ScriptableContainers will
	 * get initialized even if they don't have scripts (because we
	 * want to be able to use the script engine to show what functions
	 * are available for the script designer.
	 * returns false if there was an initialization error.
	 */
	bool initScripting(bool forDesign = false);
	/*! \brief Calls reset on the scriptables serialized into this container.
	 */
	void resetScriptableValues();

	virtual QString assetType(){return "ScriptableContainer";};
	virtual QString getInfo();

protected:
	void runScript(QString scriptName);
	void runScript(QString scriptName, bool& scriptReturnVal);
	virtual QString defaultTagName(){return "Scriptables";};
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool hasScripts(){return false;};
	//This returns a map of QMap<script name,script code>
	virtual QMap<QString,QString> getScripts(){return QMap<QString,QString>();};
	virtual void scriptableContainerWasReinitialized(){};
	QSharedPointer<AssetFactory> scriptableFactory_;
	QSharedPointer<QScriptEngine> qsEngine_;

private:
	bool bindToScriptEngine(QScriptEngine &engine);
	QList<QSharedPointer<Scriptable> > scriptables_;
	QList<QSharedPointer<ScriptableContainer> > scriptableContainers_;
	QMap<QString,QString> scriptableListProperties_;
	bool scriptingInitialized_;
	bool initializedForDesign_;

private slots:
	//This is called if something about a scriptable changed, so that the script
	//engines will need to be reinitialized before use.
	void deinitScripting();
};


}; //namespace Picto

#endif
