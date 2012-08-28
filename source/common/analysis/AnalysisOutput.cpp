#include <QDir>
#include <QCoreApplication>
#include "AnalysisOutput.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

QString AnalysisOutput::outputDir_("");
int AnalysisOutput::loadedObjects_(0);
QSharedPointer<QFile> AnalysisOutput::lockFile_;
AnalysisOutput::AnalysisOutput()
{
	loadedObjects_++;
	initTempOutputDir();
}

AnalysisOutput::~AnalysisOutput()
{
}

QString AnalysisOutput::getTempOutputDir()
{
	//int tempDirNum = 1;
	//while(QFile::exists(outputDir_+QString("/%1").arg(tempDirNum)))
	//	tempDirNum++;
	return outputDir_+QString("/%1").arg(runUuid_);
}

void AnalysisOutput::postDeserialize()
{
	AnalysisTool::postDeserialize();
}

bool AnalysisOutput::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTool::validateObject(xmlStreamReader))
		return false;
	return true;
}

//Will create an empty AnalysisOutput directory in the runpath once
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