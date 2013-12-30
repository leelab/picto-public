#ifndef _SWITCH_ELEMENT_H_
#define _SWITCH_ELEMENT_H_

#include <QSharedPointer>
#include <QCoreApplication>

#include "../common.h"
#include "StateMachineElement.h"
#include "../engine/PictoEngine.h"

namespace Picto {

/*!	\brief A state machine element that switches the control flow path based on the results of a script.
 *
 *	\details SwitchElement objects are used when a StateMachine needs to switch its control flow path based on the Experiment's current
 *	data state. A script is run that has access to all of the in scope data in its StateMachine.  This script must return a value that 
 *	is equal to the name of one of its child results.  The selected result is used as the source for the next transition in control flow.
 *
 *	\note For the purpose of design clarity we don't allow the SwitchElement Script to change any of the StateMachine's data.  If it does
 *	so a runtime error is triggered.  In testing, we found that this greatly improved design clarity by assuring that all logic that can
 *	affect StateMachine data is located in Entry, Exit, or Frame Scripts.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SwitchElement : public StateMachineElement
#else
class SwitchElement : public StateMachineElement
#endif
{
	Q_OBJECT
public:
	SwitchElement();
	virtual ~SwitchElement(){};
	static QSharedPointer<Asset> Create();

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	virtual QString slaveRun(QSharedPointer<Engine::PictoEngine> engine);

	void setScript(QString script);

	virtual QString getUITemplate(){return "SwitchElement";};
	virtual QString friendlyTypeName(){return "Switch";};

protected:
	virtual QString getReturnValueError(QString scriptName,const QScriptValue& returnValue);
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	//This returns a map of QMap<script name,script code>
	virtual QMap<QString,QString>  getScripts();

};


}; //namespace Picto

#endif
