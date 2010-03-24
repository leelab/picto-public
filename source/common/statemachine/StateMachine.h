/*!	\Brief A container for StateMachineElements that acts as a statemachine
 *
 *	A StateMachine contains multiple StateMachineElements and the
 *	transitions between them.  Additionally, a StateMachine is a 
 *	StateMachineElement, so it can be contained by other StateMachines.
 *
 *	Each state machine has a "level" (currently limited to Stage, Trial, Task).
 *	The level is used for multiple purposes:
 *		1. So the StateMachine knows when to generate startTrial/stopTrial events
 *		2. So the user is forced to think in a certain hierarchy
 *		3. So the engine knows when it is OK to spend a few CPU cycles sending
 *		   data back to the server (for example, we don't want to drop frames
 *		   between stages, but it's OK to drop frames between trials)
 *
 *	The levels are defined as follows:
 *	TASK:	This would be a full-blown "experiment".  For example, a task might
 *			include 20 memory saccade trial followed by 60 stop signal trials.
 *			A project will likely include a few tasks, but it is unlikely that
 *			an experimenter would run multiple tasks in a single day
 *	TRIAL:	A trial is a frequently repeated piece of an experiment.  Each trial 
 *			probably lasts no more than 30 seconds.
 *	STAGE:	Stages are used to organize trials.  For example, a memory saccade
 *			trial invloves a fixation stage, a sample stage, a memory stage, and
 *			a testing stage.
 *
 *	There is no technical limitation on nesting these different levels of StateMachines
 *	(for example, a stage could contain a task), however, the GUI is expected to enforce
 *	some amount of rational ordering.  Additionally, the validateStateMachine function
 *	will check that the hierarchy makes sense.  It seems best to limit nesting so that 
 *	each level is only allowed to contain the level below it, with the exception of stages,  
 *	which would be allowed to contain an infinite number of substages.
 */
#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "../common.h"
#include "StateMachineElement.h"
#include "Transition.h"

#include <QScriptEngine>

namespace Picto {

namespace StateMachineLevel
{
	typedef enum
	{
		Stage,
		Trial,
		Task,
		Experiment
	} StateMachineLevel;
}

#if defined WIN32 || defined WINCE
class PICTOLIB_API StateMachine : public StateMachineElement
#else
class StateMachine : public StateMachineElement
#endif
{
public:
	StateMachine();

	void addTransition(QSharedPointer<Transition> transition);
	void addParameter(QSharedPointer<Parameter> parameter);
	void addElement(QSharedPointer<StateMachineElement> element);
	bool setInitialElement(QString elementName);

	void setLevel(StateMachineLevel::StateMachineLevel level);
	StateMachineLevel::StateMachineLevel getLevel();

	bool validateStateMachine();

	QString run();
	bool initScripting(QScriptEngine &qsEngine);

	//DataStore functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void sendStartTrialToServer();
	void sendEndTrialToServer();

	QMultiMap<QString, QSharedPointer<Transition> > transitions_; //<source, transition>
	QMap<QString, QSharedPointer<StateMachineElement> > elements_;

	//This is used to keep track of the local parameters, so that I don't
	//serialize all the parameters every time.
	ParameterContainer localParameterContainer_;

	QScriptEngine qsEngine_;
	bool scriptingInit_;

	QStringList levelEnumStrs_;

	static int trialEventCode_;
	static int trialNum_;


};


}; //namespace Picto

#endif
