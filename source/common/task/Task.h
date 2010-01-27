#ifndef _TASK_H_
#define _TASK_H_

#include "../common.h"
#include "../storage/DataStore.h"
#include "../parameter/ParameterContainer.h"
#include "../property/PropertyContainer.h"
#include "../statemachine/StageResult.h"
#include "../experimentalsystem/ExperimentalSystem.h"
#include "Constraint.h"
#include "TrialResult.h"
#include "BlockGenerator.h"

#include <QUuid>
#include <QList>

namespace Picto {

class Task : public DataStore
{
public:
	Task();

private:
	QUuid uuid_;
	unsigned int revision_;

	PropertyContainer propertyContainer_;
	ParameterContainer parameterContainer_;
	QList<StageResult> stageResults_;
	QList<TrialResult> trialResults_;
	QList<Constraint> constraints_;
	ExperimentalSystem experimentalSystemRequirements_;
	BlockGenerator blockGenerator_;
};


}; //namespace Picto

#endif
