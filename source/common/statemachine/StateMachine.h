#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "../common.h"
#include "MachineContainer.h"
#include "Transition.h"
#include "../engine/PictoEngine.h"

namespace Picto {

/*!	\brief A container for a State Machine defined by StateMachineElement objects connected by Transitions.
 *
 *	\details A StateMachine contains multiple StateMachineElements and the Transitions between them.  It is also a
 *	StateMachineElement, itself such that it can be contained by other StateMachines.  This class manages traversal of the
 *	StateMachine inside its run() function.  Essentially, all Logic for a Task is contained inside its StateMachine, so
 *	this is one of the more important classes in the Picto framework.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API StateMachine : public MachineContainer
#else
class StateMachine : public MachineContainer
#endif
{
	Q_OBJECT
public:
	StateMachine();
	virtual ~StateMachine(){};

	static QSharedPointer<Asset> Create();

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	virtual QString slaveRun(QSharedPointer<Engine::PictoEngine> engine);

	/*! \brief Sets the stored path to this StateMachine.
	 *	\note This function has been here a long time, but since then we have added methods for automatically retrieving
	 *	an Asset's Path (Asset::getPath()), so it is probably redundant and removable.
	*/
	void setPath(QStringList path) { path_ = path; };

	bool jumpToState(QStringList path, QString state);
	virtual QString getUITemplate(){return "StateMachine";};
	virtual QString friendlyTypeName(){return "State Machine";};

	virtual void upgradeVersion(QString deserializedVersion);

protected:
	virtual QString defaultTagName(){return "StateMachine";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();

private:
	QString runPrivate(QSharedPointer<Engine::PictoEngine> engine, bool slave);

	static short trialEventCode_;
	static int trialNum_;

	QSharedPointer<StateMachineElement> currElement_;
	bool ignoreInitialElement_;
	bool dontRunElement_;
	QStringList path_;


};


}; //namespace Picto

#endif
