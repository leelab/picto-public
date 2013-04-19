#ifndef _TASK_H_
#define _TASK_H_

#include "../common.h"
#include "../statemachine/scriptablecontainer.h"
#include "../statemachine/UIEnabled.h"
//#include "../parameter/ParameterContainer.h"
#include "../property/PropertyContainer.h"
#include "../statemachine/StateMachine.h"
//#include "../statemachine/StageResult.h"
//#include "../experimentalsystem/ExperimentalSystem.h"
//#include "Constraint.h"
//#include "TrialResult.h"
//#include "BlockGenerator.h"
#include "../engine/PictoEngine.h"

//#include <QUuid>
#include <QList>

namespace Picto {

/*!	\brief A task is a single statemachine that is part of an experiment
 *
 *	An experiment is likely to contain multiple tasks.  Each task is a single
 *	activity for the experiment.  A task can contain only one StateMachine.
 *
 *	MATT'S NOTE:
 *	When I started to write this, there were a lot of contained objects that I 
 *	wasn't totally sure how to deal with.  Rather than ignoring them, I commented
 *	them out.  It is highly likely that we will need to add these as we go...
*/

#if defined WIN32 || defined WINCE
class PICTOLIB_API Task : public ScriptableContainer
#else
class Task : public UIEnabled
#endif
{
	Q_OBJECT
public:
	Task();
	virtual ~Task(){};

	static QSharedPointer<Task> Create();

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	bool jumpToState(QStringList path, QString state);
	virtual QString getUITemplate(){return "Task";};
	virtual QString assetType(){return "Task";};
	QUuid getTaskId(){return propertyContainer_->getPropertyValue("TaskId").toUuid();};
	void setTaskNumber(int num);

signals:
	void taskIdChanged(QUuid newId);

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
	bool taskIdBeingEdited_;

private slots:
	void changeTaskId();
};


}; //namespace Picto

#endif
