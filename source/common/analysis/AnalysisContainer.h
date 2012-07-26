#ifndef _ANALYSIS_CONTAINER_H_
#define _ANALYSIS_CONTAINER_H_

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
class PICTOLIB_API AnalysisContainer : public UIEnabled
#else
class AnalysisContainer : public UIEnabled
#endif
{
	Q_OBJECT
public:
	AnalysisContainer();
	virtual ~AnalysisContainer();
	static QSharedPointer<Asset> Create();

	void loadSession(QSqlDatabase session,bool enScriptDebugging);

	int numAnalysisDefs();
	QString analysisDefName(int index);
	void activateAnalysis(int index);
	void deactivateAllAnalyses();
	//AnalysisContainer specific functions
	//Resets the AnalysisContainer to its initial state
	//void reset();
	//Marks that a new run has started.  Periods will update everything that
	//uses the run name.
	void startNewRun(QSharedPointer<TaskRunDataUnit> runInfo);
	//Runs the AnalysisContainer analysis from the current run's firstIndex to its last index.
	//Note that this function may called multiple times if the lastIndex of the run is not
	//yet available.
	bool run();
	void finish();
	//bool saveOutputToDirectory(QString directory, QString filename);
	QLinkedList<QPointer<AnalysisOutputWidget>> getOutputWidgets();
	unsigned int getPercentRemaining();

	//Inherited
	virtual QString getUITemplate(){return "AnalysisContainer";};
	virtual QString assetType(){return "AnalysisContainer";};

protected:

	//Inherited
	virtual QString defaultTagName(){return "AnalysisContainer";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QSqlDatabase session_;
private:
	QVector<bool> activatedAnalyses_;

};
}; //namespace Picto
#endif