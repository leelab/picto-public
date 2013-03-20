#ifndef _SWITCH_ELEMENT_H_
#define _SWITCH_ELEMENT_H_

#include <QSharedPointer>
#include <QCoreApplication>

#include "../common.h"
#include "StateMachineElement.h"
#include "../engine/PictoEngine.h"

namespace Picto {

/*!	\brief A state machine element that switches the control flow path based on the results of a script
 *
 *	Switch elements are used when a state machine needs to switch its path based on the experiments current
 *	data state. A script is run that has access to all of the parameters of the state machine that contains 
 *	the SwitchElement.  This script must return a value that is equal to the name of one of its child results.
 *	the selected result is used as the source for the next transition in state machine control flow.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API SwitchElement : public StateMachineElement
#else
class SwitchElement : public StateMachineElement
#endif
{
public:
	SwitchElement();
	virtual ~SwitchElement(){};
	static QSharedPointer<Asset> Create();

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	virtual QString slaveRun(QSharedPointer<Engine::PictoEngine> engine);

	void setScript(QString script);

	virtual QString getUITemplate(){return "SwitchElement";};
	virtual QString assetType(){return "SwitchElement";};

protected:
	virtual QString getReturnValueError(QString scriptName,const QScriptValue& returnValue);
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	//This returns a map of QMap<script name,script code>
	virtual QMap<QString,QPair<QString,QString>>  getScripts();

};


}; //namespace Picto

#endif
