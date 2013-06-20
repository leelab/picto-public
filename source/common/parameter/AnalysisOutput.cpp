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

void AnalysisOutput::reset()
{	
	AnalysisVariable::reset();
	wasReset_ = true;
	updateWidgetValidity();
}

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
			output.toStrongRef()->updateWidgetValidity();	//We put this both here and in reset in case reset was called before this was
															//on any given AnalysisOutput object.
		}
	}
	return newContainer;
}

//If this object was reset then we know that it was in scope
//during this experimental run.  If it has a widget identifier, we know that it created a widget.
//If both are true, tell the world that its widget is valid.
void AnalysisOutput::updateWidgetValidity()
{
	if(wasReset_ && !widgetIdentifier_.isNull())
		emit widgetIsValid(widgetIdentifier_);
}

QUuid AnalysisOutput::getWidgetIdentifier()
{
	return widgetIdentifier_;
}

void AnalysisOutput::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

QString AnalysisOutput::getTempOutputDir()
{
	return outputDir_+QString("/%1").arg(getDesignConfig()->getLatestRunId().toString());
}

QString AnalysisOutput::getRunName()
{
	return getDesignConfig()->getLatestRunName();
}

QUuid AnalysisOutput::getCurrRunId()
{
	return getDesignConfig()->getLatestRunId();
}

void AnalysisOutput::runStarted(QUuid runId)
{
	if(outputsByRunId_.contains(latestRunId_))
		outputsByRunId_.remove(latestRunId_);
	latestRunId_ = runId;
	if(getDesignConfig()->getActiveAnalysisIds().contains(getAssociateId()))
		outputsByRunId_[latestRunId_].append(selfPtr().staticCast<AnalysisOutput>());
}

void AnalysisOutput::runEnded()
{
	finishUp();
	wasReset_ = false;
}

AnalysisOutputWidget* AnalysisOutput::loadWidget()
{
	AnalysisOutputWidget* returnVal = createWidget();
	if(!returnVal)
		return NULL;
	widgetIdentifier_ = QUuid::createUuid();
	return returnVal;
}
//Will create an empty AnalysisOutputDep directory in the runpath once
//per executable run, clearing out any previous contents if there 
//were any.
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
	Q_ASSERT(rc);

}

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
