#include <QDir>
#include <QCoreApplication>
#include "AnalysisOutputDep.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

QString AnalysisOutputDep::outputDir_("");
int AnalysisOutputDep::loadedObjects_(0);
QSharedPointer<QFile> AnalysisOutputDep::lockFile_;
AnalysisOutputDep::AnalysisOutputDep()
{
	loadedObjects_++;
	initTempOutputDir();
}

AnalysisOutputDep::~AnalysisOutputDep()
{
}

QString AnalysisOutputDep::getTempOutputDir()
{
	//int tempDirNum = 1;
	//while(QFile::exists(outputDir_+QString("/%1").arg(tempDirNum)))
	//	tempDirNum++;
	return outputDir_+QString("/%1").arg(runUuid_.toString());
}

void AnalysisOutputDep::postDeserialize()
{
	AnalysisTool::postDeserialize();
}

bool AnalysisOutputDep::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTool::validateObject(xmlStreamReader))
		return false;
	return true;
}

//Will create an empty AnalysisOutputDep directory in the runpath once
//per executable run, clearing out any previous contents if there 
//were any.
void AnalysisOutputDep::initTempOutputDir()
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

void AnalysisOutputDep::removeFilesThenDirectories(QDir container)
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