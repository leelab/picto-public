#ifndef _ANALYSIS_DEFINITION_H_
#define _ANALYSIS_DEFINITION_H_

#include <QSqlDatabase>
#include <QWidget>
#include <QPointer>
#include "../statemachine/UIEnabled.h"
#include "AnalysisPeriod.h"
#include "AnalysisOutputWidget.h"

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
	void startNewRun(QString runName);
	//Runs the AnalysisDefinition analysis from the fromIndex to the toIndex.
	//Note that if this function is called multiple times before a reset, they
	//must be in increasing order (ie. Each new fromIndex must be greater than
	//the toIndex from the preceding call).
	bool run(EventOrderIndex fromIndex,EventOrderIndex toIndex);
	void finish();
	bool outputCanBeSaved();
	//bool saveOutputToDirectory(QString directory, QString filename);
	QLinkedList<QPointer<AnalysisOutputWidget>> getOutputWidgets();

	//Inherited
	virtual QString getUITemplate(){return "AnalysisDefinition";};
	virtual QString assetType(){return "AnalysisDefinition";};

signals:
	void percentRemaining(int percent);

protected:

	//Inherited
	virtual QString defaultTagName(){return "AnalysisDefinition";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QSqlDatabase session_;
private:
	unsigned int currPeriod_;
	unsigned int numPeriods_;

private slots:
	void updateProgressBar(int periodPercentRemaining);

};
}; //namespace Picto
#endif