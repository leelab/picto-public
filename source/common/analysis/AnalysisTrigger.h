#ifndef _ANALYSIS_TRIGGER_H_
#define _ANALYSIS_TRIGGER_H_

#include <QLinkedList>
#include <QVector>
#include <QScriptValue>
#include <QScriptEngine>
#include <QSqlDatabase>
#include "../statemachine/UIEnabled.h"
#include "EventOrderIndex.h"
#include "AnalysisDataIterator.h"
#include "AnalysisDataSourceDep.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisTrigger : public UIEnabled
#else
class AnalysisTrigger : public UIEnabled
#endif
{
	Q_OBJECT
public:
	AnalysisTrigger();
	virtual ~AnalysisTrigger();

	void loadSessionAndScriptTools(QSqlDatabase session,QSharedPointer<QScriptEngine> qsEngine);
	void setDataWindow(EventOrderIndex startFrom,EventOrderIndex endBefore);


	//AnalysisTrigger specific functions
	//Resets the AnalysisTrigger to its initial state
	//void reset();
	//Sets the start index of the current analysis period.  Internally, this
	//causes the AnalysisTrigger to clear all data from its arrays that occured
	//before this index.  It must always increase until reset() is called, although
	//in the case of "negative buffers" in the AnalysisPeriod it may be set to an index
	//lower than a prior input to fillArraysTo().
	void setPeriodStart(EventOrderIndex startIndex);

	//Sets the index before which this analysis trigger should fill its data arrays.
	//values filled in will be up to but not including beforeIndex.
	//The input value must always increase until reset() is called.
	void fillArraysTo(EventOrderIndex beforeIndex);

	QString scriptInfo();
	virtual EventOrderIndex::IDSource getDataSource() = 0;

	//Returns the Event Order Index at the occurence of the next trigger.
	//If no more triggers are available, it will return an invalid
	//EventOrderIndex.
	EventOrderIndex getNextTrigger();

	//Returns the Event Order Index that was returned at the most recent
	//getNextTrigger() call.
	EventOrderIndex getCurrentTrigger();

	void sessionDatabaseUpdated();

	//Should be implemented to return the fraction of triggers that still need to
	//be processed for this session (that this object knows about).
	float fractionTriggersRemaining();

	QSharedPointer<AnalysisValue> getLatestValue();
	QString getIteratorDescriptor();

	//Inherited
	virtual QString getUITemplate(){return "AnalysisTrigger";};
	virtual QString friendlyTypeName(){return "AnalysisTrigger";};

protected:

	virtual QSharedPointer<AnalysisDataIterator> createDataIterator() = 0;

	//Inherited
	virtual QString defaultTagName(){return "AnalysisTrigger";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	QSqlDatabase session_;
	QSharedPointer<QScriptEngine> qsEngine_;

private:
	QSharedPointer<AnalysisValue> latestValue_;
	EventOrderIndex currentTriggerIndex_;
	EventOrderIndex periodStart_;
	struct SourceDataUnit
	{
		QString name;
		QString value;
	};
	struct TriggerData
	{
		TriggerData(EventOrderIndex id,QScriptValue script){index=id;rowScript=script;};
		EventOrderIndex index;
		QScriptValue rowScript;
		/*QVector<SourceDataUnit> dataBlock;*/
	};
	QLinkedList<TriggerData> periodData_;
	QLinkedList<TriggerData>::iterator nextPeriodsDataLoc_;
	QSharedPointer<AnalysisDataIterator> dataIterator_;
	QScriptValue triggerArray_;
};
}; //namespace Picto
#endif