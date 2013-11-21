#ifndef _AnalysisOutput_H_
#define _AnalysisOutput_H_

#include <QDir>
#include <QMutex>
#include <QPointer>
#include <QUuid>
#include <QHash>
#include <QList>
#include <QWeakPointer>
#include "../common.h"

#include "AnalysisVariable.h"
#include "AnalysisOutputWidgetContainer.h"
#include "AnalysisOutputWidget.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisOutput : public AnalysisVariable
#else
class AnalysisOutput : public AnalysisVariable
#endif
{
	Q_OBJECT

public:
	AnalysisOutput();
	virtual ~AnalysisOutput();

	virtual void enteredScope();

	virtual QString getUITemplate(){return "AnalysisOutput";};
	virtual QString getUIGroup(){return "Outputs";};

	virtual void setDesignConfig(QSharedPointer<DesignConfig> designConfig);

	//Note.  If you call this, it's your job to delete it the widget that
	//you get
	static AnalysisOutputWidgetContainer* getContainerWidget(QUuid runId);

	void updateWidgetValidity();
	QUuid getWidgetIdentifier();
signals:
	//This is emitted whenever this AnalysisObject's reset function is called.
	void widgetIsValid(QUuid widgetIdentifier);
protected:
	virtual void postDeserialize();
	virtual QString defaultTagName(){return "AnalysisOutput";};

	//Warning: This may get called from a thread that this AnalysisOutput is not in.
	//The widget might also be accessed from a thread that the AnalysisOutput is not int.
	virtual AnalysisOutputWidget* createWidget(){return NULL;};

	QString getTempOutputDir();
	QString getRunName();
	QUuid getCurrRunId();

	//Called to end the analysis.  Should be overloaded to finish any operations that
	//need to be finished before analysis ends.
	virtual void finishUp(){};

protected slots:
	void runStarted(QUuid runId);
	void runEnded();

private:
	AnalysisOutputWidget* loadWidget();
	QUuid widgetIdentifier_;
	QUuid latestRunId_;
	bool wasReset_;
	static QHash<QUuid,QList<QWeakPointer<AnalysisOutput>>> outputsByRunId_;
	static void initTempOutputDir();
	static void removeFilesThenDirectories(QDir container);
	static QString outputDir_;
	static int loadedObjects_;
	static QSharedPointer<QFile> lockFile_;	//File that is kept open while the application is active to prevent the directory from being deleted.
	static QMutex containerWidgetMutex_;
	static QHash<QUuid,QPointer<AnalysisOutputWidgetContainer>> containerWidgets_;
	bool valid_;
};


}; //namespace Picto

#endif
