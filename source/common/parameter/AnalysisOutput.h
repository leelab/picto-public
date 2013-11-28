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

/*! \brief AnalysisOutput objects are the endpoint of Analysis data.  Any data captured and manipulated
 *	in the Analysis, will finally be written to an AnalysisOutput for presentation to the viewer.
 *	\details An AnalysisOutput might be as simple as an AnalysisFileOutput with which text can be written to a
 *	file.  It might also be as complex as a specific type of raster output which might output a pdf file containing
 *	a raster plot.  Data is written to AnalysisOutput objects with simple javascript functions exposed by the each
 *	type of AnalysisOutput class just like any other element in a Picto design.
 *
 *	AnalysisOutput objects should also be associated with an AnalysisOutputWidget that is used to present their data to the
 *	operator during an Analysis.  The specific type of AnalysisOutput class is responsible for defining the correct
 *	type of widget to be used with it.  AnalysisOutput objects are responsible for creating their associated widget and 
 *	filling it with data.  The AnalysisOutput class keeps track of all of the AnalysisOutput objects that are participating 
 *	in a particular run.  This allows it to create an AnalysisOutputWidgetContainer that packages all of the 
 *	AnalysisOutputWidgets into a single parent widget for display in the Picto environment.
 *	
 *	Data saved by AnalysisOutput objects is stored underneath the "analysisoutput" directory in the Picto run
 *	directory.  Each time Picto starts, this directory is erased meaning that storage there is temporary.  AnalysisOutputWidget
 *	objects take care of copying data from that temporary directory to a permanent location on the operator's request.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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

	//Note.  If you call this, it's your job to delete the widget that
	//you get
	static AnalysisOutputWidgetContainer* getContainerWidget(QUuid runId);

	void updateWidgetValidity();
	QUuid getWidgetIdentifier();
signals:
	/*! \brief Emitted when this AnalysisObject's enteredScope() function is called if it has an associated valid
	 *	widget.
	 *	\details widgetIdentifier is a unique id associated with this AnalysisObject's widget.
	 */
	void widgetIsValid(QUuid widgetIdentifier);
protected:
	virtual void postDeserialize();
	virtual QString defaultTagName(){return "AnalysisOutput";};

	/*! \brief Creates a widget that displays this AnalysisOutput object's contents and returns it.
	 *	\warning This function may be called from a thread that this AnalysisOutput is not in.
	 *	The widget itself nmight also be accessed from a thread that the AnalysisOutput is not in.
	 */
	virtual AnalysisOutputWidget* createWidget(){return NULL;};

	QString getTempOutputDir();
	QString getRunName();
	QUuid getCurrRunId();

	/*! \brief Called to end the analysis.  Should be overloaded to finish any operations that
	 *	need to be finished before analysis ends.
	 *	\details In the case of an analysis object that writes to a file, for example, all of the 
	 *	latest contents in the file buffer could be flushed from within this function.
	 */
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
