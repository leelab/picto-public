#include <QCoreApplication>
#include <QFileInfo>
#include <QDateTime>
#include "AutoSaver.h"
#include "AutoSaveDialog.h"
#include "../memleakdetect.h"
#define LOCKFILENAME "LockFile"
#define AUTOSAVEROOT "AutoSave"
namespace Picto {

/*! \brief We never need more than one AutoSaver, but we do need it to use signals and slots, so we implement it as a
 *	singleton.
 */
QWeakPointer<AutoSaver> AutoSaver::singleton_ = QWeakPointer<AutoSaver>();

/*! \brief Constructs a new AutoSaver.  Calls initAutoSaveDir() in the process.
 */
AutoSaver::AutoSaver()
: QObject()
{
	initAutoSaveDir();
	saveTimer_.stop();
	connect(&saveTimer_,SIGNAL(timeout()),this,SLOT(saveDesignToFile()));
}
/*! \brief Returns the singleton instance of the AutoSaver object.*/
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
/*! \brief When the AutoSaver is deleted, we close our lockFile_ and designFile_ then remove the AutoSave subdirectory
 *	for this workstation.
 *	\details The idea here is that if the AutoSaver is being smoothly deleted, there is no need for the AutoSaved file.
 *	It is only necessary in the event of a crash.
 */
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
/*! \brief Starts autosaving the current design to file every savePeriodMs.*/
void AutoSaver::start(int savePeriodMs)
{
	saveTimer_.stop();
	saveTimer_.setInterval(savePeriodMs);
	saveTimer_.start();
}
/*! \brief Saves the design to file, then stops AutoSaving the current design at a periodic interval. */ 
void AutoSaver::stop()
{
	saveTimer_.stop();
	saveDesignToFile();	//This will pick up any last changes before stop was called.
}

/*! \brief Sets the designRoot of the design that will be auto-saved to file.
 *	\details The designRoot will be saved fo file each time saveDesignToFile() is called by the saveTimer_
 *	so long as something about the design has changed.  The saveFirstVersion input tells the AutoSaver to 
 *	save the input designRoot right away and not to wait for the next change to be made before saving the file
 * for the first time
 */
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

/*! \brief Deletes the auto-saved file and updates the saved file text such that no auto-saved file will exist
 *	until the next change is made to the design.
 */
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

/*! \brief This function is used with the Qt sorting algorithm to order file paths by alphabetical file name only.
*/
bool putPathsWithLatestFileNamesFirst(const QString &s1, const QString &s2)
{
	return QFileInfo(s1).fileName().toLower() > QFileInfo(s2).fileName().toLower();
}
/*! \brief Constructs and returns an AutoSaveDialog to allow the designer to restore an auto saved design file.
 *	This function connects up the AutoSaveDialog signals such that the delete and restore operations requested
 *	by the dialog will be properly performed.  If there are no auto-saved files available, the function returns
 *	NULL.
 */
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

/*! \brief Returns the path where this AutoSaver's files should be saved (a sub-directory of [PictoInstallDir]/AutoSave).
*/
QString AutoSaver::getAutoSaveDir()
{
	return autoSaveDir_;
}

/*! \brief Gathers information from and initializes the AutoSave directory for auto-saving and restoring of previously auto-saved files.
 *	\details The AutoSaver object creates a sub directory in [PictoInstallDir]/AutoSave to put its auto-saved design file in.  It
 *	also creates a "lock file" in that directory which is removed along with the design file when the object is deleted on closing
 *	the workstation.  When initAutoSaveDir is called, it checks all sub-directories of [PictoInstallDir]/AutoSave.  If any have lock 
 *	files, it attempts to remove them.  If it succeeds that means that the workstation that created the lock file is no longer open and
 *	using it, and that it crashed since otherwise that directory would no longer be there.  Therefore, once the LockFile removal has been
 *	attempted, this object can go through all the AutoSave subdirectories and every file that is found in a directory with no lock file
 *	is an auto-saved file that should be available for restoring.  Paths to all files of this kind are added to a list for usage in a
 *	AutoSaveWidget.
 */
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
	IGNORED_PARAMETER(rc);
	Q_ASSERT(rc);
}

/*! \brief This is a useful funciton for removing directories and all contents.  We created it before Qt 5.0 when
 *	there was not Qt version of a function that would do this.  I believe that there is one now, so that can be 
 *	used the next time this class is refactored.
 */
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
/*! \brief Creates an automatic name for an autosaved design file by using the current 
 *	date and time (yyyy_MM_dd__hh_mm_ss_)+designName+.xml.
 *	\details This way, alphabetized auto-saved version can appear in the 
 *	order of their creation.
 */
QString AutoSaver::buildDesignFileName(const QString designName)
{
	QDateTime dateTime = QDateTime::currentDateTime();
	return dateTime.toString("yyyy_MM_dd__hh_mm_ss_") + designName + ".xml";
}

/*! \brief Saves the current version of the design to file if anything has changed since the last save.
 */
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
		IGNORED_PARAMETER(rc);
		Q_ASSERT(rc);
	}
}

/*! \brief Called when requested by the AutoSaveWidget to delete an auto-saved file at the input path and its 
 *	[PictoInstallDir]/AutoSave sub-directory.
 */
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
/*! \brief Called when requested by the AutoSaveWidget to open the auto-saved file at the input path.
 *	\note Once the design has been opened, the auto-saved file is deleted.  The designer must press save
 *	to save this restored design; however, since the restored version will also be auto-saved there isn't
 *	too much danger of losing the restored file if the designer forgets to save the restored version.
 */
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
