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
#include "../parameter/AssociateRootHost.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisContainer : public UIEnabled, public AssociateRootHost
#else
class AnalysisContainer : public UIEnabled, public AssociateRootHost
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
	virtual QString friendlyTypeName(){return "AnalysisContainer";};
	//We're getting rid of this and don't want it to interfere with validation of the Design
	//as a whole, so this just always returns true.
	virtual bool validateTree(){return true;};

	ASSOCIATE_ROOT_HOST_PUBLIC_IMPLEMENTATION
protected:

	//Inherited
	virtual QString defaultTagName(){return "AnalysisContainer";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QSqlDatabase session_;
private:
	QVector<bool> activatedAnalyses_;

	bool hostIdBeingEdited_;
private slots:
	void changeHostId(){if(hostIdBeingEdited_) return;hostIdBeingEdited_ = true;propertyContainer_->setPropertyValue("HostId",QUuid::createUuid());hostIdBeingEdited_ = false;};

};
}; //namespace Picto
#endif