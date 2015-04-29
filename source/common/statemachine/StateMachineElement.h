#ifndef _STATEMACHINEELEMENT_H_
#define _STATEMACHINEELEMENT_H_
#include <QObject>
#include <QPoint>
#include <QRect>
#include <QSharedPointer>
#include <QStringList>

#include "../common.h"

#include "../parameter/Parameter.h"
#include "../property/PropertyContainer.h"
#include "../storage/DataStore.h"
#include "../engine/PictoEngine.h"
#include "../network/CommandChannel.h"
#include "../protocol/ProtocolResponse.h"
#include "../statemachine/result.h"
#include "ResultContainer.h"

namespace Picto {

class Result;

/*!	\brief A StateMachineElement is any element that can be part of the control flow in a StateMachine.
 *	
 *	StateMachineElements include things like States, Results, SwitchElements, and even StateMachines themselves.  Each
 *	StateMachineElement implements a run() function that executes its internal logic and is called whenever control flow
 *	transfers to it.  All StateMachineElements also have EntryScript and ExitScript Properties that define scripts that
 *	are called when the StateMachine is entered (when a Transition to it is triggered) or exited (when control flow
 *	reaches one of its results).
 *
 *	StateMachineElements are connected by Transitions objects with each Transition starting at a StateMachineElement's
 *	Result and ending at another StateMachineElement.
 *
 *	Scoping in StateMachineElements works as follows.  Scripts have access to all Scriptable elements that are direct
 *	children of their StateMachineElement parent or any of its ancestor StateMachineElement objects.  Attached
 *	AnalysisScripts have the same scoping except that they can also access attached Scriptable AnalysisElements (regular
 *	Scripts cannot access AnalysisElements).  If two Scriptable elements have the same name and are both in scope, the
 *	scriptable with the longer path will be the one that is accessed by using its name.
 *
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API StateMachineElement : public ResultContainer
#else
class StateMachineElement : public ResultContainer
#endif
{
	Q_OBJECT

public:
	StateMachineElement();
	virtual ~StateMachineElement(){};


	/*! \brief Called when control flow reaches this StateMachineElement to trigger its internal execution logic.
	 *	\details The returned value is the name of the Result of the StateMachineElement's execution logic.
	 */
	virtual QString run(QSharedPointer<Engine::PictoEngine> engine) = 0;
	/*! \brief This function comes from back before we were using the SlaveExperimentDriver.  
	 *	\details Since the SlaveExperimentDriver handles changing of Property values and transitioning control flow based
	 *	on what happened in the Master experiment, and slaveRenderFrame() handles the rendering component, the slaveRun()
	 *	function itself is pretty much obsolete at this point.  It should probably be removed, but since some classes use
	 *	it to perform various minor operations, we will need to assure that they are moved to more appropriate functions
	 *	first.
	 */
	virtual QString slaveRun(QSharedPointer<Engine::PictoEngine> engine) = 0;
	virtual QString slaveRenderFrame(QSharedPointer<Engine::PictoEngine> engine);

	virtual QString friendlyTypeName(){return "State Machine Element";};

	virtual QString getUITemplate(){return "StateMachineElement";};
	virtual QString getUIGroup(){return "State Machine Elements";};

protected:
	virtual QString defaultTagName(){return "StateMachineElement";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	void runEntryScript();
	void runExitScript();
	void runAnalysisEntryScripts();
	void runAnalysisExitScripts();
	enum ScriptType {ENTRY,FRAME,EXIT};
	void runAnalysisScripts(ScriptType type);
	virtual QMap<QString,QString> getScripts();

	QPoint layoutPosition_;
	friend class SlaveExperimentDriver;

};


}; //namespace Picto

#endif
