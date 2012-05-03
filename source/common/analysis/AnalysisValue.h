#ifndef _ANALYSIS_VALUE_H_
#define _ANALYSIS_VALUE_H_

#include <QScriptEngine>
#include <QScriptValue>
#include "EventOrderIndex.h"

namespace Picto {
//AnalysisValue is the parent class for all AnalysisDataStore values that 
//are read in from the session.
struct AnalysisValue
{
	AnalysisValue(QSharedPointer<QScriptEngine> qsEngine, EventOrderIndex id = EventOrderIndex()){index=id;if(qsEngine)scriptVal = qsEngine->newObject();};
	EventOrderIndex index;
	QScriptValue scriptVal;
};

}; //namespace Picto
#endif