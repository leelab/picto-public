#ifndef _SCRIPT_CONTROLLER_H_
#define _SCRIPT_CONTROLLER_H_

#include "../common.h"
#include "ControlElement.h"

namespace Picto {

/*! \brief A general ControlElement with user definable logic and results
 *	\details A ScriptController is a broad use controller in which the designer can 
 *	create their own logical checks for whether to continue in a state or exit from it.  
 *	Like in a SwitchElement, you can add multiple LogicResult elements to a ScriptController 
 *	and trigger them by returning a string that is equal to one of the results's names.  In 
 *	order to continue in the State without exiting, simply return any value that is not equal 
 *	to a result name or nothing at all.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API ScriptController : public ControlElement
#else
class ScriptController : public ControlElement
#endif
{
	Q_OBJECT
public:
	ScriptController();
	virtual ~ScriptController(){};

	static ControlElement* NewScriptController();
	static QSharedPointer<Asset> Create();
	static QString ControllerType();

	bool isDone(QSharedPointer<Engine::PictoEngine> engine);
	QSharedPointer<Result> getResult();
	void start(QSharedPointer<Engine::PictoEngine> engine);
	virtual void upgradeVersion(QString deserializedVersion);

	virtual QString getUITemplate(){return "ScriptController";};
	virtual QString friendlyTypeName(){return "Script Controller";};
	////DataStore Functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
protected:
	virtual QString getReturnValueError(QString scriptName,const QScriptValue& returnValue);
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	//This returns a map of QMap<script name,script code>
	virtual QMap<QString,QString>  getScripts();

private:
	QString currResult_;
};


}; //namespace Picto

#endif
