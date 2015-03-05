#include <QCoreApplication>
#include "AnalysisOutput.h"
#include "../memleakdetect.h"

namespace Picto {

QString AnalysisOutput::outputDir_("");
int AnalysisOutput::loadedObjects_(0);
QSharedPointer<QFile> AnalysisOutput::lockFile_;
QHash<QUuid,QList<QWeakPointer<AnalysisOutput>>> AnalysisOutput::outputsByRunId_;
QMutex AnalysisOutput::containerWidgetMutex_;
QHash<QUuid,QPointer<AnalysisOutputWidgetContainer>> AnalysisOutput::containerWidgets_;

AnalysisOutput::AnalysisOutput()
: AnalysisVariable()
{
	loadedObjects_++;
	initTempOutputDir();
	wasReset_ = false;
}

AnalysisOutput::~AnalysisOutput()
{
	if(outputsByRunId_.contains(latestRunId_))
		outputsByRunId_.remove(latestRunId_);
}

void AnalysisOutput::enteredScope()
{	
	AnalysisVariable::enteredScope();
	wasReset_ = true;
	updateWidgetValidity();
}

/*! \brief Extends AnalysisVariable::setDesignConfig() to connect the DesignConfig's runEnded() and runStarted()
 *	functions to this objects runEnded() and runStarted() functions.
 */
void AnalysisOutput::setDesignConfig(QSharedPointer<DesignConfig> designConfig)
{
	//When the design config emits a runEnded event, we should call finishUp to
	//finalize any output activities (ie. flush file contents, etc).
	if(getDesignConfig())
	{
		disconnect(getDesignConfig().data(),SIGNAL(runEnded()),this,SLOT(runEnded()));
		disconnect(getDesignConfig().data(),SIGNAL(runStarted(QUuid)),this,SLOT(runStarted(QUuid)));
	}
	AnalysisVariable::setDesignConfig(designConfig);
	connect(getDesignConfig().data(),SIGNAL(runEnded()),this,SLOT(runEnded()));
	connect(getDesignConfig().data(),SIGNAL(runStarted(QUuid)),this,SLOT(runStarted(QUuid)));
}

/*! \brief Returns an AnalysisOutputWidgetContainer containing all of the AnalysisOutputWidget objects
 *	associated with the run that has the input runId.
 *	\details The AnalysisOutput class tracks all AnalysisOutput objects that are created in order to be
 *	able to provide all AnalysisOutputWidgets for a given run. When this function is called, a new
 *	AnalysisOutputWidgetContainer is created and loaded.  Widgets are requested for all AnalysisOutput 
 *	objects for the input runId by calling loadWidget() and they are added to the AnalysisOutputWidgetContainer
 *	before it is returned.
 *	If no AnalysisOutput object are associated with the inputn runId, a Null pointer will be returned.
 *	\note The class calling this function is responsible for deleting the returned AnalysisOutputWidgetContainer.
 */
AnalysisOutputWidgetContainer* AnalysisOutput::getContainerWidget(QUuid runId)
{
	if(!outputsByRunId_.contains(runId))
		return NULL;
	AnalysisOutputWidgetContainer* newContainer = NULL;
	foreach(QWeakPointer<AnalysisOutput> output,outputsByRunId_.value(runId))
	{
		if(output.isNull())
			continue;
		if(!newContainer)
			newContainer = new AnalysisOutputWidgetContainer(output.toStrongRef()->getRunName());

		AnalysisOutputWidget* widget = output.toStrongRef()->loadWidget();
		if(widget)
		{
			newContainer->addWidget(output.toStrongRef()->getWidgetIdentifier(),widget);
			connect(output.data(),SIGNAL(widgetIsValid(QUuid)),newContainer,SLOT(widgetIsValid(QUuid)));
			output.toStrongRef()->updateWidgetValidity();	//We put this both here and in enteredScope in case enteredScope was called before this was
															//on any given AnalysisOutput object.
		}
	}
	return newContainer;
}

/*! \brief Each time updateWidgetValidity() is called, we check to see if the widget has entered scope since
 *	the latest run started and if it has a widget.  If both are true, we emit widgetIsValid() with the widgets
 *	identifier.
 */
 void AnalysisOutput::updateWidgetValidity()
{
	if(wasReset_ && !widgetIdentifier_.isNull())
		emit widgetIsValid(widgetIdentifier_);
}

 /*! \brief Returns a QUuid associated with the widget that displays this AnalysisOutput's contents.
  *	\details If this AnalysisOutput doesn't have a widget for displaying its contents, this returns an
  *	invalid QUuid.
  */
QUuid AnalysisOutput::getWidgetIdentifier()
{
	return widgetIdentifier_;
}

void AnalysisOutput::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

/*! \brief Returns the directory where directories and files written from this AnalysisOutput will be temporarily stored
 *	for this run.
 *	\details These directories and files will be deleted the next time Picto starts.
 */
QString AnalysisOutput::getTempOutputDir()
{
	return outputDir_+QString("/%1").arg(getDesignConfig()->getLatestRunId().toString());
}

/*! \brief Returns the name of the currently active run.
*/
QString AnalysisOutput::getRunName()
{
	return getDesignConfig()->getLatestRunName();
}

/*! \brief Returns the RunID ofht currently active run.
*/
QUuid AnalysisOutput::getCurrRunId()
{
	return getDesignConfig()->getLatestRunId();
}

/*! \brief Called when a new run starts to initialize this AnalysisOutput.  
*	\details Checks if this object's parent Analysis is active, and if so adds
 *	this object to the class's static list of AnalysisOutputs for the input runId so that the object's
 *	associated widget can be included in the AnalysisOutputWidgetContainer returned from getContainerWidget().
 *	\sa getContainerWidget(), AnalysisOutputWidgetContainer
 */
void AnalysisOutput::runStarted(QUuid runId)
{
	if(outputsByRunId_.contains(latestRunId_))
		outputsByRunId_.remove(latestRunId_);
	latestRunId_ = runId;
	QList<QUuid> activeAnalysisIds = getDesignConfig()->getActiveAnalysisIds();
	if(activeAnalysisIds.size() && activeAnalysisIds.contains(getAssociateId()))
		outputsByRunId_[latestRunId_].append(selfPtr().staticCast<AnalysisOutput>());
}

/*! \brief Called when the current run ends to deinitialize this AnalysisOutput.
 *	\details Internally, calls finishUp()
 */
void AnalysisOutput::runEnded()
{
	finishUp();
	wasReset_ = false;
}

/*! \brief Calls createWidget to create a new Widget to display this AnalysisOutput's contents and returns it.
 *	\details If this AnalysisOutput has no associated widget, a NULL pointer is returned.  Also, when a widget
 *	is created, a unique ID is generated for the widget that can be accessed from getWidgetIdentifier().  This
 *	will allow the widget to be uniquely identifiable in an AnalysisOutputWidgetContainer since the widget
 *	identifier is used in AnalysisOutputWidgetContainer::addWidget() to add the widget.
 *	\sa getContainerWidget()
 */
AnalysisOutputWidget* AnalysisOutput::loadWidget()
{
	AnalysisOutputWidget* returnVal = createWidget();
	if(!returnVal)
		return NULL;
	widgetIdentifier_ = QUuid::createUuid();
	return returnVal;
}
/*! \brief Creates an empty "analysisoutput" directory in the runpath once
 *	per executable run, clearing out any previous contents if there were any.
 *	\details This allows the analysisoutput directory to act as a temporary container
 *	for any analysis output files created by the Picto workstation.  If the data stored in
 *	the "analysisoutput" directory needs to be saved to a permanent location, this can be
 *	done by calling the AnalysisOutputWidget::saveOutputTo() function which will be 
 *	responsible for copying its associated data files from the "analysisoutput" directory to the 
 *	directory path input in that function.
 *	\sa AnalysisOutputWidget::saveOutputTo(), AnalysisOutputWidgetContainer::saveOutputTo()
 */
void AnalysisOutput::initTempOutputDir()
{
	//We want to make sure that even if multiple Picto Workstations are open at the
	//same time, no one will remove the data that we are currently using.  We also 
	//want to erase the data if we are just opening for the first time and no one else
	//is using the data in the temporary folder.  For this reason, we attempt to remove
	//all files at the top level of the temporary directory, then if successful, remove
	//all other contained directories as well.  We then create a top level file in the
	//temporary directory and leave it open until the application closes.
	//If another workstation is open, we won't succeed in removing its open file so we
	//wont delete the temporary directory contents, otherwise, we will.
	if(!outputDir_.isEmpty())
		return;
	QDir dir(QCoreApplication::applicationDirPath()+"/../");
	dir.mkdir("analysisoutput");
	dir.cd("analysisoutput");
	removeFilesThenDirectories(dir);

	outputDir_ = dir.absolutePath();

	QString fileName = outputDir_+"/"+QUuid::createUuid().toString();
	lockFile_ = QSharedPointer<QFile>(new QFile(fileName));
	bool rc = lockFile_->open(QIODevice::WriteOnly | QIODevice::Text);
	IGNORED_PARAMETER(rc);
	Q_ASSERT(rc);

}

/*! \brief A convenience function that deletes all files within the input directory
 *	and then deletes the directory itself.
 *	\details If any files in the directory can't be removed, the function returns
 *	and nothing else is touched.
 *	\note This was created since, at the time of its writing, Qt had no single function
 *	for performing a directory deletion.  The latest version of Qt should have this
 *	function, so we may want to replace this with the Qt version.
 */
void AnalysisOutput::removeFilesThenDirectories(QDir container)
{
	QStringList files = container.entryList(QDir::Files);
	foreach(QString fileName,files)
	{
		if(!container.remove(fileName))
		{
			return;
		}
	}
	QStringList directories = container.entryList(QDir::Dirs|QDir::NoDotAndDotDot|QDir::NoSymLinks);
	foreach(QString directory,directories)
	{
		if(!container.rmdir(directory))
		{
			QDir subDir = container;
			subDir.cd(directory);
			removeFilesThenDirectories(subDir);
			container.rmdir(directory);
		}
	}
}

}; //namespace Picto
