/*	\brief A task is a single statemachine that is part of an experiment
 *
 *	An experiment is likely to contain multiple tasks.  Each task is a single
 *	activity for the experiment.  A task can contain only one StateMachine.
 *	
 *
 *	MATT'S NOTE:
 *	When I started to write this, there were a lot of contained objects that I 
 *	wasn't totally sure how to deal with.  Rather than ignoring them, I commented
 *	them out.  It is highly likely that we will need to add these as we go...
*/

#ifndef _TASK_H_
#define _TASK_H_

#include "../common.h"
#include "../storage/DataStore.h"
#include "../parameter/ParameterContainer.h"
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

#if defined WIN32 || defined WINCE
class PICTOLIB_API Task : public DataStore
#else
class Task : public DataStore
#endif
{
public:
	Task();

	QString name();
	void setName(QString name);

	bool run(QSharedPointer<Engine::PictoEngine> engine);

	//DataStore Functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void sendFinalStateDataToServer(QString result, QSharedPointer<Engine::PictoEngine> engine);

	//QUuid uuid_;
	//unsigned int revision_;

	PropertyContainer propertyContainer_;
	ParameterContainer parameterContainer_;
	QSharedPointer<StateMachine> stateMachine_;
	//QList<StageResult> stageResults_;
	//QList<TrialResult> trialResults_;
	//QList<Constraint> constraints_;
	//ExperimentalSystem experimentalSystemRequirements_;
	//BlockGenerator blockGenerator_;
};


}; //namespace Picto

#endif
