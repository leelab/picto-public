#ifndef _ANALYSIS_DEFINITION_H_
#define _ANALYSIS_DEFINITION_H_

#include <QSqlDatabase>
#include <QWidget>
#include <QPointer>
#include <QScriptEngine>
#include "../statemachine/UIEnabled.h"
#include "AnalysisPeriod.h"
#include "AnalysisOutputWidget.h"
#include "../storage/TaskRunDataUnit.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisDefinition : public UIEnabled
#else
class AnalysisDefinition : public UIEnabled
#endif
{
	Q_OBJECT
public:
	AnalysisDefinition();
	virtual ~AnalysisDefinition();

	void loadSession(QSqlDatabase session);

	//AnalysisDefinition specific functions
	//Resets the AnalysisDefinition to its initial state
	void reset();
	//Marks that a new run has started.  Periods will update everything that
	//uses the run name.
	void startNewRun(QSharedPointer<TaskRunDataUnit> runInfo);
	//Runs the AnalysisDefinition analysis from the current run's firstIndex to its last index.
	//Note that this function may called multiple times if the lastIndex of the run is not
	//yet available.
	bool run();
	void finish();
	//bool saveOutputToDirectory(QString directory, QString filename);
	QLinkedList<QPointer<AnalysisOutputWidget>> getOutputWidgets();
	unsigned int getPercentRemaining();

	//Inherited
	virtual QString getUITemplate(){return "AnalysisDefinition";};
	virtual QString assetType(){return "AnalysisDefinition";};

protected:

	//Inherited
	virtual QString defaultTagName(){return "AnalysisDefinition";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QSqlDatabase session_;
private:
	double getFrameTime(qulonglong frameId);
	QSharedPointer<TaskRunDataUnit> currRun_;
	QSharedPointer<QScriptEngine> qsEngine_;
	QSharedPointer<AnalysisPeriod> currPeriod_;
	unsigned int currPeriodNum_;
	unsigned int numPeriods_;
	unsigned int currRunNum_;

};
}; //namespace Picto
#endif