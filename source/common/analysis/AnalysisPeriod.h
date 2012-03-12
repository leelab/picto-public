#ifndef _ANALYSIS_PERIOD_H_
#define _ANALYSIS_PERIOD_H_

#include <QSqlDatabase>
#include <QScriptValue>
#include <QWidget>
#include <QPointer>
#include "../statemachine/UIEnabled.h"
#include "EventOrderIndex.h"
#include "AnalysisTrigger.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisPeriod : public UIEnabled
#else
class AnalysisPeriod : public UIEnabled
#endif
{
	Q_OBJECT
public:
	AnalysisPeriod();
	virtual ~AnalysisPeriod();
	static QSharedPointer<Asset> Create();

	void loadSession(QSqlDatabase session);

	//AnalysisPeriod specific functions
	//Resets the AnalysisPeriod to its initial state
	void reset();
	//Runs the analysisPeriod analysis from the last time runTo was called
	//(or time zero if we just reset) to the input time value.  To run
	//the analysisPeriod until the are no more AnalysisPeriods available 
	//in the session, input a negative number.
	bool runTo(double time);
	QLinkedList<QPointer<QWidget>> getOutputWidgets();

	//Inherited
	virtual QString getUITemplate(){return "AnalysisPeriod";};
	virtual QString assetType(){return "AnalysisPeriod";};

signals:
	void percentRemaining(int percent);

protected:

	//Inherited
	virtual QString defaultTagName(){return "AnalysisPeriod";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QSqlDatabase session_;

private:
	QString scriptInfo();
	QSharedPointer<AnalysisTrigger> startTrigger_;
	QSharedPointer<AnalysisTrigger> endTrigger_;
	EventOrderIndex startIndex_;
	EventOrderIndex endIndex_;
	QLinkedList<QPointer<QWidget>> outputWidgets_;
};
}; //namespace Picto
#endif