#ifndef _ANALYSIS_TRIGGER_H_
#define _ANALYSIS_TRIGGER_H_

#include <QLinkedList>
#include <QVector>
#include <QScriptValue>
#include <QScriptEngine>
#include <QSqlDatabase>
#include "../statemachine/UIEnabled.h"
#include "EventOrderIndex.h"

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

	void loadSession(QSqlDatabase session);

	//AnalysisTrigger specific functions
	//Resets the AnalysisTrigger to its initial state
	void reset();
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

	void addScriptableArrayToEngine(QSharedPointer<QScriptEngine> qsEngine);

	QString scriptInfo();

	//Should be implemented by child classes to get the next trigger time
	//following the last one returned.  After restart() is called, it will return
	//the first trigger time in the session.  If there are no more triggers available
	//it should return a negative value.
	virtual EventOrderIndex getNextTriggerTime() = 0;

	//After this function is called, the first trigger in the session should 
	//be returned from getNextTriggerTime()
	virtual void restart() = 0;

	//Should be implemented to return the total number of triggers in this session
	//that this object knows about.
	virtual unsigned int totalKnownTriggers() = 0;
	//Should be implemented to return the number of triggers that still need to
	//be processed for this session (that this object knows about).
	virtual unsigned int remainingKnownTriggers() = 0;

	//Inherited
	virtual QString getUITemplate(){return "AnalysisTrigger";};
	virtual QString assetType(){return "AnalysisTrigger";};

protected:

	//Inherited
	virtual QString defaultTagName(){return "AnalysisTrigger";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QSqlDatabase session_;

private:
	EventOrderIndex periodStart_;
	struct SourceDataUnit
	{
		QString name;
		QString value;
	};
	struct TriggerData
	{
		EventOrderIndex index;
		QVector<SourceDataUnit> dataBlock;
	};
	QLinkedList<TriggerData> periodData_;
	QLinkedList<TriggerData>::iterator nextPeriodsDataLoc_;
};
}; //namespace Picto
#endif