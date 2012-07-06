#ifndef _SCRIPTFUNCTION_H_
#define _SCRIPTFUNCTION_H_

#include <QVariant>
#include <QScriptValue>

#include "../common.h"
#include "../statemachine/ScriptableContainer.h"

namespace Picto {

/*!	\brief A base class for storing values that can be used in a state machine
 *
 *	Since Picto requires users to build state machines to define their experiments, we need
 *	some sort of "variable" (actually we don't, but variables make the state machines a lot
 *	simpler).  The ScriptFunction object is that variable.  The base class includes functions
 *	for converting ScriptFunctions to XML, as well as the comparison operators as virtual 
 *	functions.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API ScriptFunction : public ScriptableContainer
#else
	class ScriptFunction : public ScriptableContainer
#endif
{
	Q_OBJECT

public:
	ScriptFunction();

	static QSharedPointer<Asset> Create();
	virtual ~ScriptFunction(){};
	bool bindToScriptEngine(QScriptEngine &engine);
	virtual QString getUITemplate(){return "ScriptFunction";};
	virtual QString assetType(){return "ScriptFunction";};
	virtual QString getScriptingInfo();

public slots:
	//setters and getters are slots so we can bind them to scripts


protected:
	virtual QString defaultTagName(){return "ScriptFunction";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
};


}; //namespace Picto

#endif
