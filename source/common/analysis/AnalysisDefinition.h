#ifndef _ANALYSIS_DEFINITION_H_
#define _ANALYSIS_DEFINITION_H_

#include <QSqlDatabase>
#include <QWidget>
#include <QPointer>
#include "../statemachine/UIEnabled.h"
#include "AnalysisPeriod.h"

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
	//Runs the AnalysisDefinition analysis from the last time runTo was called
	//(or time zero if we just reset) to the input time value.  To run
	//the AnalysisDefinition until the are no more AnalysisDefinitions available 
	//in the session, input a negative number.
	bool runTo(double time);
	void finish();
	QLinkedList<QPointer<QWidget>> getOutputWidgets();

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