#ifndef _ANALYSIS_PERIOD_H_
#define _ANALYSIS_PERIOD_H_

#include <QSqlDatabase>
#include <QScriptValue>
#include <QWidget>
#include <QPointer>
#include <QList>
#include "../statemachine/UIEnabled.h"
#include "EventOrderIndex.h"
#include "AnalysisTrigger.h"
#include "AnalysisOutputWidget.h"

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
	//Marks that a new run has started.  Period will update everything that
	//uses the run name.
	void startNewRun(QString runName);
	//Runs the analysisPeriod analysis from the fromIndex to the toIndex.
	//Note that if this function is called multiple times before a reset, they
	//must be in increasing order (ie. Each new fromIndex must be greater than
	//the toIndex from the preceding call).
	bool run(EventOrderIndex fromIndex,EventOrderIndex toIndex);
	void finishUp();
	QLinkedList<QPointer<AnalysisOutputWidget>> getOutputWidgets();
	//bool saveOutputToDirectory(QString directory, QString filename);

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
	EventOrderIndex getNextTriggerInList(QString tagName, EventOrderIndex afterIndex);
	QString scriptInfo();
	QList<QSharedPointer<AnalysisTrigger>> startTriggers_;
	QList<QSharedPointer<AnalysisTrigger>> endTriggers_;
	EventOrderIndex startIndex_;
	EventOrderIndex endIndex_;
	unsigned int periodNumber_;
	QLinkedList<QPointer<QWidget>> outputWidgets_;
	QSharedPointer<QScriptEngine> qsEngine_;
};
}; //namespace Picto
#endif