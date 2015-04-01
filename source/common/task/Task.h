#ifndef _TASK_H_
#define _TASK_H_

#include "../common.h"
#include "../statemachine/scriptablecontainer.h"
#include "../statemachine/UIEnabled.h"
#include "../property/PropertyContainer.h"
#include "../statemachine/StateMachine.h"
#include "../engine/PictoEngine.h"

#include <QList>

namespace Picto {

/*!	\brief A Task contains the top level StateMachine that defines the logic for researching an
 *	experimental question.
 *
 *	A Task can define anything from a MemorySaccade exercise, to a system for checking the calibration of a reward supply
 *	system.  An Experiment usually contains multiple Tasks with each Task containing only one StateMachine.  The period
 *	from the time a Task starts until it finishes is called a Task Run, or simply a Run.  The Task class defines all
 *	the setup/take down logic for the Task Run and starts the underlying StateMachine that handles its actual experimental
 *	control logic.
 *
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API Task : public ScriptableContainer
#else
class Task : public ScriptableContainer
#endif
{
	Q_OBJECT
public:
	Task();
	virtual ~Task(){};

	static QSharedPointer<Task> Create();

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	virtual bool hasEditableDescendants(){return true;};
	virtual QString getUITemplate(){return "Task";};
	virtual QString friendlyTypeName(){return "Task";};
	virtual QString getUIGroup(){return "State Machine Elements";};
	void setTaskNumber(int num);
	/*! \brief Returns a pointer to the top level StateMachine contained in this Task.
	*/
	QSharedPointer<StateMachine> getStateMachine(){return stateMachine_;};

protected:
	virtual QString defaultTagName(){return "Task";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void sendInitialStateDataToServer(QSharedPointer<Engine::PictoEngine> engine);
	void sendFinalStateDataToServer(QString result, QSharedPointer<Engine::PictoEngine> engine);

	QSharedPointer<StateMachine> stateMachine_;
	int taskNumber_;
	QSharedPointer<Transition> initTransition_;
};


}; //namespace Picto

#endif
