#ifndef _SCRIPTFUNCTION_H_
#define _SCRIPTFUNCTION_H_

#include <QVariant>
#include <QScriptValue>

#include "../common.h"
#include "../statemachine/ScriptableContainer.h"

namespace Picto {

/*!	\brief An element of the Picto StateMachine used to define a Script function that can be called from
 *	any script for the purpose of reusing the same code in multiple places.
 *	\details Objects of this class have an Inputs Property and a Script Property.  In the Input Property
 *	the designer enters a comma separated list of function input parameters.  The Script Property is the
 *	actual logic that is run whenever the ScriptFunction is called.  The Script function is called by using
 *	its name inside another script, and inputting whatever parameters are needed to match the values in
 *	the Inputs Property.  The Script Property logic can then use the parameter values by using the names 
 *	entered in that field.  For example, if a ScriptFunction were called "SampleFunction" and defined
 *	with:
 *		- Inputs : "input1, input2"
 *		- Script: "return input1+input2;"
 *	
 *	A script in Picto for which SampleFunction is in scope can use the following code:
 *	\code
		var addedValue = SampleFunction(23,12);
		var isThirtyFive = (addedValue == 35); //will be true.
	\endcode
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
	bool hasContentsForScriptEngine();
	bool bindToScriptEngine(QScriptEngine &engine);
	virtual QString getUITemplate(){return "ScriptFunction";};
	virtual QString friendlyTypeName(){return "Script Function";};
	virtual QString getUIGroup(){return "Logic Elements";};
	virtual QString getScriptingInfo();
	virtual void upgradeVersion(QString deserializedVersion);

protected:
	virtual QString defaultTagName(){return "ScriptFunction";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
	virtual bool executeSearchAlgorithm(SearchRequest searchRequest);
};


}; //namespace Picto

#endif
