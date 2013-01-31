#include <QDir>
#include <QCoreApplication>
#include "BufferFileGenerator.h"

#include "../../common/memleakdetect.h"
using namespace Picto;
#define TEMPDIRNAME "TemporaryFiles"

QString BufferFileGenerator::outputDir_("");
QSharedPointer<QFile> BufferFileGenerator::lockFile_;

QSharedPointer<QFile> BufferFileGenerator::createTempFile(QString subDirName, QString fileType)
{
	//Assure that the top level output directory exists
	initTempOutputDir();
	//Assure that the sub directory where this generators temp files will be located exists.
	QString outputSubDir = createSubDirectory(subDirName);
	//Make sure that our sub directory is available
	Q_ASSERT(!outputSubDir.isEmpty());
	//Create a file using a unique identifier followed by "." and the fileType
	QString fileName = outputSubDir+"/"+QUuid::createUuid().toString()+"."+fileType;
	return QSharedPointer<QFile>(new QFile(fileName));
}

//Creates a subdirectory within the staticly generated temporary directory
QString BufferFileGenerator::createSubDirectory(QString subDirName)
{
	Q_ASSERT(!outputDir_.isEmpty());
	QDir dir(outputDir_);
	dir.mkdir(subDirName);
	dir.cd(subDirName);
	return dir.absolutePath();
}

//Will create an empty BufferFileGenerator directory in the runpath once
//per executable run, clearing out any previous contents if there 
//were any.
void BufferFileGenerator::initTempOutputDir()
{
	//We want to make sure that even if multiple Picto applications are open at the
	//same time, no one will remove the data that we are currently using.  We also 
	//want to erase the data if we are just opening for the first time and no one else
	//is using the data in the temporary folder.  For this reason, we attempt to remove
	//all files at the top level of the temporary directory, then if successful, remove
	//all other contained directories as well.  We then create a top level file in the
	//temporary directory and leave it open until the application closes.
	//If another workstation is open, we won't succeed in removing its open file so we
	//wont delete the temporary directory contents, otherwise, we will.

	//If an output directory has already been created, initialization is done
	if(!outputDir_.isEmpty())
		return;

	//If the output directory doesn't exist, create it.
	QDir dir(QCoreApplication::applicationDirPath()+"/../");
	dir.mkdir(TEMPDIRNAME);
	
	//Go to the output directory
	dir.cd(TEMPDIRNAME);

	//Remove everything in the directory if no one else is using it
	removeFilesThenDirectories(dir);

	//Set our output directory to this one.
	outputDir_ = dir.absolutePath();

	//Create a lock file so that no one else will delete the output directory while
	//we're using it.
	QString fileName = outputDir_+"/"+QUuid::createUuid().toString();
	lockFile_ = QSharedPointer<QFile>(new QFile(fileName));
	bool rc = lockFile_->open(QIODevice::WriteOnly | QIODevice::Text);
	Q_ASSERT(rc);

}

//Removes everything in the input directory and all of its sub directories.
//Stops at any level where a file cannot be removed.
void BufferFileGenerator::removeFilesThenDirectories(QDir container)
{
	//Get all files in this directory.
	QStringList files = container.entryList(QDir::Files);
	//Attempt to remove each file.
	foreach(QString fileName,files)
	{
		//If a file cannot be removed, stop recursion now.
		if(!container.remove(fileName))
		{
			return;
		}
	}
	//Get all sub directories
	QStringList directories = container.entryList(QDir::Dirs|QDir::NoDotAndDotDot|QDir::NoSymLinks);
	//Recurse into each sub directory to remove all files.  Afterwards remove sub-directory.
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