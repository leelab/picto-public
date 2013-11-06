#include <QCoreApplication>
#include <QFileInfo>
#include <QDateTime>
#include "AutoSaver.h"
#include "../memleakdetect.h"
#define LOCKFILENAME "LockFile"
#define AUTOSAVEROOT "AutoSave"
namespace Picto {

QWeakPointer<AutoSaver> AutoSaver::singleton_ = QWeakPointer<AutoSaver>();

AutoSaver::AutoSaver()
: QObject()
{
	initAutoSaveDir();
}

QSharedPointer<AutoSaver> AutoSaver::create()
{
	QSharedPointer<AutoSaver> returnVal = singleton_.toStrongRef();
	if(returnVal.isNull())
	{
		returnVal = QSharedPointer<AutoSaver>(new AutoSaver());
		singleton_ = returnVal;
	}
	return returnVal;
}

AutoSaver::~AutoSaver()
{
	//If we created an autoSaveDirectory
	if(autoSaveDir_.length())
	{
		//Close the lock file and designFiles, since we're done with it.
		if(designFile_ && designFile_->isOpen())
		{
			designFile_->close();
		}
		if(lockFile_ && lockFile_->isOpen())
		{
			lockFile_->close();
		}
		//If this object is being closed smoothly, it means that the user
		//is closing the application smoothly and will not need autosaved
		//files, so we delete our autosave directory.
		QDir subDir(autoSaveDir_);
		removeFilesThenDirectories(subDir);
		subDir.rmdir(".");
	}
}

void AutoSaver::setDesignRoot(QSharedPointer<DesignRoot> designRoot)
{
	//Store the design root
	designRoot_ = designRoot;
	//Get the new file name for this design
	QString designName;
	if(designRoot_)
		designName = designRoot_->getDesignName();
	QString newFileName = buildDesignFileName(designName);
	//If there's no design file, create it now
	if(autoSaveDir_.isEmpty())
		return;
	if(!designFile_ && designRoot_)
	{
		QString designFilePath = QDir(autoSaveDir_).filePath(newFileName);
		designFile_ = QSharedPointer<QFile>(new QFile(designFilePath));
		bool rc = designFile_->open(QIODevice::ReadWrite | QIODevice::Text);
		Q_ASSERT(rc);
	}
	saveDesignToFile();
}

void AutoSaver::saveDesignToFile()
{
	//If we are here and there's no designFile, there must have been a problem
	//creating it (read only directory or something)
	if(!designFile_)
		return;
	//Get the latest design code;
	QString newDesignCode;
	if(designRoot_)
		newDesignCode = designRoot_->getDesignRootText();
	//If the design code hasn't changed, we're done
	if(newDesignCode == latestDesignCode_)
		return;
	//Update latestDesignCode_
	latestDesignCode_ = newDesignCode;
	//If the latestDesignCode_ is empty, remove the designFile and we're done
	if(latestDesignCode_.isEmpty())
	{
		designFile_->close();
		designFile_->remove();
		return;
	}
	//Update contents of the design file with the latest design code
	designFile_->resize(0);
	designFile_->write(latestDesignCode_.toLatin1());
	designFile_->flush();
	//If the name of the design root has changed, update the name of 
	//the design file
	QString newDesignName;
	if(designRoot_)
		newDesignName = designRoot_->getDesignName();
	QString newFileName = buildDesignFileName(newDesignName);
	QString newDesignFilePath = QDir(autoSaveDir_).filePath(newFileName);
	if(designFile_->fileName() != newDesignFilePath)
	{
		designFile_->rename(newDesignFilePath);	//This closes the file
		bool rc = designFile_->open(QIODevice::ReadWrite | QIODevice::Text);
		Q_ASSERT(rc);
	}
}

QSharedPointer<DesignRoot> AutoSaver::takePreviouslyAutoSavedDesign()
{
	QSharedPointer<DesignRoot> design;
	//Go through the saved design directories list looking for a design.
	//Keep going until the list is empty or we have a valid design.
	while(design.isNull() && savedDesignDirs_.size())
	{
		//Get the directory of the first saved design and remove it
		//from the list
		QDir designDir(savedDesignDirs_.takeFirst());
		//Get the file with the saved design
		QStringList files = designDir.entryList(QDir::Files);
		QString savedFilePath = files.length()?designDir.filePath(files.first()):"";
		//Load what's in the savedFilePath into a fileContents string
		QString fileContents;
		QString fileName;
		if(savedFilePath.length())
		{
			QFile savedFile(savedFilePath);
			bool rc = savedFile.open(QIODevice::ReadOnly | QIODevice::Text);
			if(rc)
			{
				fileContents = QString(savedFile.readAll());
				fileName = QFileInfo(savedFile).fileName();
				savedFile.close();
			}
		}
		//Create a DesignRoot from the FileContents string
		if(fileContents.length())
		{
			design = QSharedPointer<DesignRoot>(new DesignRoot());
			if(!design->resetDesignRoot(fileContents))
				design.clear();
			else
				design->setDesignName(fileName);
		}
		//Remove the directory that contained the autosaved file.
		removeFilesThenDirectories(designDir);
		designDir.rmdir(".");
	}
	return design;
}

QString AutoSaver::getAutoSaveDir()
{
	return autoSaveDir_;
}

void AutoSaver::initAutoSaveDir()
{
	//Create a directory object and position it at the autosave root directory
	Q_ASSERT(autoSaveDir_.isEmpty());
	QDir autoSaveRoot(QCoreApplication::applicationDirPath()+"/../");
	autoSaveRoot.mkdir(AUTOSAVEROOT);
	if(!autoSaveRoot.cd(AUTOSAVEROOT))
		return;
	//Go through auto save directories
	QStringList subDirectoryNames = autoSaveRoot.entryList(QDir::Dirs|QDir::NoDotAndDotDot|QDir::NoSymLinks,QDir::Time);
	foreach(QString subDirName,subDirectoryNames)
	{
		//Get the files in the subdirectory
		QDir subDir(autoSaveRoot.filePath(subDirName));
		QStringList subDirFiles = subDir.entryList(QDir::Files);
		//If the sub directory is empty, remove it
		if(subDirFiles.isEmpty())
		{
			removeFilesThenDirectories(subDir);
			subDir.rmdir(".");
			continue;
		}
		//Try to remove any lockfile in the sub directory
		subDir.remove(LOCKFILENAME);

		//If the sub directory doesn't contain contain a lock file then
		//the workstation controlling is no longer using it, so it is 
		//an autosave directory that we should add to our list
		if(!subDir.exists(LOCKFILENAME))
		{
			savedDesignDirs_.push_front(autoSaveRoot.filePath(subDirName));
		}
	}
	//We now have a list of all directores that contain auto saved files.
	
	//Create a directory that will contain auto saved files for
	//this application.
	QString newDirName = QUuid::createUuid().toString();
	if(!autoSaveRoot.mkdir(newDirName))
		return;
	autoSaveDir_ = autoSaveRoot.filePath(newDirName);
	//Create a lock file in the new directory so that other applications
	//will know that we're still using this directory
	QString lockFilePath = QDir(autoSaveDir_).filePath(LOCKFILENAME);
	lockFile_ = QSharedPointer<QFile>(new QFile(lockFilePath));
	bool rc = lockFile_->open(QIODevice::WriteOnly | QIODevice::Text);
	Q_ASSERT(rc);
}

void AutoSaver::removeFilesThenDirectories(QDir container)
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

QString AutoSaver::buildDesignFileName(const QString designName)
{
	QDateTime dateTime = QDateTime::currentDateTime();
	return dateTime.toString("yyyy_MM_dd__hh_mm_ss_") + designName + ".xml";
}

}; //namespace Picto
