#include <QCoreApplication>
#include <QFileInfo>
#include <QDateTime>
#include "AutoSaver.h"
#include "AutoSaveDialog.h"
#include "../memleakdetect.h"
#define LOCKFILENAME "LockFile"
#define AUTOSAVEROOT "AutoSave"
namespace Picto {

QWeakPointer<AutoSaver> AutoSaver::singleton_ = QWeakPointer<AutoSaver>();

AutoSaver::AutoSaver()
: QObject()
{
	initAutoSaveDir();
	saveTimer_.stop();
	connect(&saveTimer_,SIGNAL(timeout()),this,SLOT(saveDesignToFile()));
}

QSharedPointer<AutoSaver> AutoSaver::getSingleton()
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

void AutoSaver::start(int savePeriodMs)
{
	saveTimer_.stop();
	saveTimer_.setInterval(savePeriodMs);
	saveTimer_.start();
}

void AutoSaver::stop()
{
	saveTimer_.stop();
	saveDesignToFile();	//This will pick up any last changes before stop was called.
}

//saveFirstVersion tells the AutoSaver to save the input designRoot right away and not
//wait for changes to be made.  Otherwise, the design will only be saved after something
//has changed.
void AutoSaver::setDesignRoot(QSharedPointer<DesignRoot> designRoot,bool saveFirstVersion)
{
	//If the designRoot_ hasn't changed, we're done
	if(designRoot_ == designRoot)
		return;
	//Store the design root
	designRoot_ = designRoot;
	//Remove previously auto saved file
	if(designFile_)
	{
		designFile_->close();
		designFile_->remove();
	}

	if(!designRoot_)
		return;

	if(saveFirstVersion)
	{
		//Go ahead and do the first autosave of the new design
		saveDesignToFile();
	}
	else
	{
		//Set the latestDesignCode_ to the current code so that
		//saving won't start until something changes.
		latestDesignCode_ = designRoot_->getDesignRootText();
	}
}

void AutoSaver::removeFileUntilNextChange()
{
	//Set the latestDesignCode_ to the current code so that
	//saving won't restart until something changes.
	latestDesignCode_ = designRoot_->getDesignRootText();
	if(designFile_)
	{
		designFile_->close();
		designFile_->remove();
	}
}

 bool putPathsWithLatestFileNamesFirst(const QString &s1, const QString &s2)
 {
     return QFileInfo(s1).fileName().toLower() > QFileInfo(s2).fileName().toLower();
 }

QDialog* AutoSaver::getDesignRestoreDialog(QWidget* parent)
{
	if(!savedDesignDirs_.size())
		return NULL;
	//Make a list of filePaths of saved designs using our list of autosave directories
	QStringList filePaths;
	foreach(QDir designDir,savedDesignDirs_)
	{
		//Get the file with the saved design
		QStringList files = designDir.entryList(QDir::Files);
		QString savedFilePath = files.length()?designDir.filePath(files.first()):"";
		if(savedFilePath.length())
			filePaths.append(savedFilePath);
	}
	if(!filePaths.size())
		return NULL;
	//Sort filePaths by reverse file name so that the newest files will be first
	qSort(filePaths.begin(),filePaths.end(),putPathsWithLatestFileNamesFirst);

	AutoSaveDialog* autoSaveDialog(new AutoSaveDialog(filePaths,parent));
	connect(autoSaveDialog,SIGNAL(deleteFileRequest(QString)),this,SLOT(deleteRestorableDesignFile(QString)));
	connect(autoSaveDialog,SIGNAL(restoreFileRequest(QString)),this,SLOT(restoreDesignFile(QString)));
	return static_cast<QDialog*>(autoSaveDialog);
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
			savedDesignDirs_.push_front(QDir(autoSaveRoot.filePath(subDirName)));
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

void AutoSaver::saveDesignToFile()
{
	//Make sure that there is a designRoot_ to save to file
	if(!designRoot_)
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
	//If the latestDesignCode_ is empty, remove the designFile if it exists 
	//and we're done
	if(latestDesignCode_.isEmpty())
	{
		if(designFile_)
		{
			designFile_->close();
			designFile_->remove();
		}
		return;
	}

	//Get the latest design name and build an updated file name from it
	QString newDesignName;
	if(designRoot_)
		newDesignName = designRoot_->getDesignName();
	QString newFileName = buildDesignFileName(newDesignName);
	QString newDesignFilePath = QDir(autoSaveDir_).filePath(newFileName);

	//If there's no design file yet, make one
	if(!designFile_)
	{
		designFile_ = QSharedPointer<QFile>(new QFile(newDesignFilePath));
		bool rc = designFile_->open(QIODevice::ReadWrite | QIODevice::Text);
		//If we didn't succesfully open the file, we can't autosave... we're done here.
		if(!rc)
			return;
	}

	//Update contents of the design file with the latest design code
	designFile_->resize(0);
	designFile_->write(latestDesignCode_.toLatin1());
	designFile_->flush();
	//If the name of the design root has changed, update the name of 
	//the design file
	if(designFile_->fileName() != newDesignFilePath)
	{
		designFile_->rename(newDesignFilePath);	//This closes the file
		bool rc = designFile_->open(QIODevice::ReadWrite | QIODevice::Text);
		Q_ASSERT(rc);
	}
}

void AutoSaver::deleteRestorableDesignFile(QString filePath)
{
	QFileInfo deleteFileInfo(filePath);
	QDir deleteFileDir = deleteFileInfo.dir();
	if(!savedDesignDirs_.contains(deleteFileDir))
		return;

	//Remove the directory that contained the autosaved file from the 
	//savedDesignDirs list and then from the file system
	savedDesignDirs_.removeAll(deleteFileDir);
	removeFilesThenDirectories(deleteFileDir);
	deleteFileDir.rmdir(".");
}

void AutoSaver::restoreDesignFile(QString filePath)
{
	//Open the file, get it's contents and name, then close it
	QFile savedFile(filePath);
	QString fileContents;
	QString fileName;
	bool rc = savedFile.open(QIODevice::ReadOnly | QIODevice::Text);
	if(rc)
	{
		fileContents = QString(savedFile.readAll());
		fileName = QFileInfo(savedFile).fileName();
		savedFile.close();
	}
	//Create a DesignRoot from the FileContents string and set its
	//name to the name of the file
	QSharedPointer<DesignRoot> design;
	if(fileContents.length())
	{
		design = QSharedPointer<DesignRoot>(new DesignRoot());
		if(!design->resetDesignRoot(fileContents))
			design.clear();
		else
			design->setDesignName(fileName);
	}

	//Remove the file and its autosave directory from the file system.
	deleteRestorableDesignFile(filePath);
	//Tell our owner to open the design
	emit openDesign(design);
}


}; //namespace Picto
