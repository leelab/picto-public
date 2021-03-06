#include <QDesktopWidget>
#include <QMessageBox>
#include <QDir>
#include <QProcess>
#include <QProgressDialog>

#include "UpdateDownloader.h"

#include "../protocol/protocolcommand.h"
#include "../protocol/protocolresponse.h"
#include "../timing/timedmessagebox.h"
#include "../mainmemleakdetect.h"

using namespace Picto;
#define RESPONSEDELAYMS 20000
#define MESSAGEBOXDELAYSEC 10
int UpdateDownloader::lastCommandId_ = 0;
QSharedPointer<UpdateDownloader> UpdateDownloader::singleton_;

UpdateDownloader::UpdateDownloader()
:
useWarning_(false),
requireInteraction_(false),
updating_(false)
{
	connect(&reattemptDiscoveryTimer_,SIGNAL(timeout()),this,SLOT(reattemptDiscovery()));
	connect(&serverVersionRequestTimer_,SIGNAL(timeout()),this,SLOT(noNewVersionAvailable()));
}

/*! \brief Returns a singleton instance of an UpdateDownloader.
 */
QSharedPointer<UpdateDownloader> UpdateDownloader::getInstance()
{
	if(singleton_.isNull())
	{
		singleton_ = QSharedPointer<UpdateDownloader>(new UpdateDownloader());
	}
	return singleton_;
}

/*! \brief Use this to set the root widget of this application.
 *	\details This is used as part of the process of closing the application when it needs to be restarted automatically after an update.
 */
void UpdateDownloader::setRootWidget(QWidget* root)
{
	rootWidgetPointer_ = root;
}

/*! \brief Use this enable an update warning allowing the user to disable the automatic update before it happens.
 *	\note By default, the update warning is disabled.
 */

void UpdateDownloader::enableUpdateWarning(bool en)
{
	useWarning_ = en;
}

/*! \brief If set true, the user will be required to press a button after an error message comes up.  If user interaction is disabled,
 *	error messages will timeout automatically after MESSAGEBOXDELEYSEC.
 *	\note By default, the user interaction is disabled.
 */
void UpdateDownloader::requireUserInteraction(bool en)
{
	requireInteraction_ = en;
}

/*! \brief Removes all files in the input path or subdirectores that ends with a ".new" or ".old" suffix.*/
bool deleteOldFiles(QString dirPath)
{
	QDir dir(dirPath);
	QStringList filters;
	filters << "*.old" << "*.new";
	dir.setNameFilters(filters);
	dir.setFilter(QDir::Files);
	QStringList files = dir.entryList();
	foreach(QString file,files)
	{
		if(!dir.remove(file))
			return false;
	}

	dir.setNameFilters(QStringList());
	dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	QFileInfoList dirKids = dir.entryInfoList();
	foreach(QFileInfo dirKid,dirKids)
	{
		if(!deleteOldFiles(dirKid.absoluteFilePath()))
			return false;
	}

	//remove directories with the .old or .new suffix
	dir.setNameFilters(filters);
	dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	dirKids = dir.entryInfoList();
	foreach(QFileInfo dirKid,dirKids)
	{
		if(!dir.rmdir(dirKid.fileName()))
		{
			return false;
		}
	}
	return true;
}

/*! \brief Call this to update this application to the latest code available on the server.
 *	@param serverAddress The IP address of the Picto Server.
 *	@param port The port on which to request the new Picto files.
 *	\details This function starts out by requesting user permission to perform the update if necessary,
 *	it then goes through the following procedure:
 *	- Deletes all files in the Picto directory with ".new" or ".old" suffixes. (deleteOldFiles())
 *	- Downloads all files from the server, unzips them and saves them with added ".new" suffixes in the application directory (getFiles())
 *	- Append ".old" suffixes to everything in the application directory that isn't ".new", removes all ".new" suffixes, starts a new
 *		instance of the application in a new process and exits the current application process. (updateApps())
 *
 *	If any of the above steps fails, an appropriate error message is displayed, and any changes that would make the current installation
 *	corrupt (ie. if the ".old" suffixes were appended to the current application files) are fixed.
 */
bool UpdateDownloader::update(QString serverAddress, quint16 port)
{
	if(updating_)
		return false;
	updating_ = true;
	if(!useWarning_ || acceptUpdate())
	{
		serverAddress_ = serverAddress;
		appUpdatePort_ = port;
		if(deleteOldFiles(QCoreApplication::applicationDirPath()))
		{
			if(getFiles())
			{
				if(!updateApps())
				{
					reportUnableToCompleteUpdate();
				}
			}
			else
			{
				reportUnableToGetFiles();
			}
		}
		else
		{
			reportUnableToRemoveOldFiles();
		}
	}
	else if(useWarning_)
	{	//User cancelled the update.  Don't bother them with automatic updates anymore this session.
		stopAutoUpdate_ = true;	
	}
	//If we got here, the program didn't exit, so we failed.
	emit updateFailed();
	updating_ = false;
	return false;
}

/*! \brief Sets the commands needed to restart this application.
 *	@param exePath The path to the running executable.
 *	@param args The list of arguments to use with the executable.
 *	\details If the application update is successfull, the application is restarted automatically.
 *	This tells the updateDownloader what commands need to be used to perform the restart.
 */
void UpdateDownloader::setRestartCommands(QString exePath,QStringList args)
{
	exePath_ = exePath;
	exeArgs_ = args;
}

/*! \brief Use this function to set up automatic checking for updates in an application that wouldn't normally be connecting to
 *	the server all the time anyway (ie. The workstation when not operating in RemoteViewer mode).
 *	\note Client applications that use CommandChannel::readIncomingResponse() trigger an autoupdate automatically due to some code in
 *	that function.
 */
void UpdateDownloader::autoCheckForUpdates(int freqSec)
{
	stopAutoUpdate_ = false;
	autoCheckFreq_ = freqSec;
	if(!serverDiscoverer_)
	{
		serverDiscoverer_ = QSharedPointer<ServerDiscoverer>(new ServerDiscoverer());
		connect(serverDiscoverer_.data(),SIGNAL(foundServer(QHostAddress, quint16)),this,SLOT(serverDiscovered(QHostAddress, quint16)));
		connect(serverDiscoverer_.data(),SIGNAL(discoverFailed()),this,SLOT(reattemptDiscovery()));
	}
	lastAutoCheckStart_ = QDateTime::currentDateTime();
	serverDiscoverer_->discover(autoCheckFreq_*1000);
}

/*!	\brief Downloads all files from the server, unzips them and saves them with added ".new" suffixes in the application directory.
*/
bool UpdateDownloader::getFiles()
{
	int totalFiles = 1000;
	QProgressDialog prog("Downloading Latest Picto Version...					","",0,totalFiles);
	prog.setWindowModality(Qt::ApplicationModal);
	prog.setCancelButton(NULL);
	prog.setMinimumDuration(0);
	//Create update socket
	updateSocket_ = QSharedPointer<QTcpSocket>(new QTcpSocket());
	//Connect to server
	updateSocket_->connectToHost(serverAddress_, appUpdatePort_, QIODevice::ReadWrite);
	if(!updateSocket_->waitForConnected(RESPONSEDELAYMS))
		return false;
	
	//Get the path of the executable directory
	QDir binDir(QCoreApplication::applicationDirPath());

	//Download and create files one by one
	Picto::ProtocolResponse response;
	int fileIndex = 0;
	updaterFileName_ = "";
	while(fileIndex < totalFiles)
	{
		//Request the next file
		if(!updateSocket_)
			break;
		Picto::ProtocolCommand command(QString("GETFILE %1 UPDATEPICTO/1.0").arg(fileIndex++));
		QString commandId = QString::number(++lastCommandId_);
		command.setFieldValue("Command-ID",commandId);
		command.write(updateSocket_.data());

		//Get response
		int r = response.read(updateSocket_.data(),RESPONSEDELAYMS);
		while((r >= 0) && (response.getFieldValue("Command-ID") != commandId))
		{
			r = response.read(updateSocket_.data(),RESPONSEDELAYMS);
		}
		if(r < 0)
			break;

		//Validate response
		Q_ASSERT(response.getResponseCode() == 200 || response.getResponseCode() == 404);
		if(response.getResponseCode() != 200 && response.getResponseCode() != 404)
			break;

		//Download the file
		QString fileName = response.getFieldValue("FileName");
		if(fileName.isEmpty())
			break;
		
		//Get totalNumFiles
		if(!response.getFieldValue("TotalFiles").isEmpty())
		{
			totalFiles = response.getFieldValue("TotalFiles").toInt();
			if(totalFiles != prog.maximum())
				prog.setMaximum(totalFiles);
		}
		
		QString dir = response.getFieldValue("Directory");
		QByteArray fileData = qUncompress(response.getDecodedContent());
		if(fileData.isEmpty())
			break;
		
		//If the file is in a sub directory, put it in that directory with .new appended to the directory name and file names
		if(!dir.isEmpty())
		{
			dir += ".new";
			//If the directory doesn't exist, create it in bin
			if(!binDir.exists(dir))
			{	//If the directory doesn't exist, make it
				if(!binDir.mkdir(dir))
					break;
			}
		}
		
		//Create the file in its appropriate subdirectory
		QFile* downloadedFile = new QFile(binDir.absolutePath() + "/"+dir+"/" + fileName + ".new");
		if(downloadedFile->open(QIODevice::Append)) 
		{
			downloadedFile->write(fileData);
			downloadedFile->close();
		}
		else
			break;
		delete downloadedFile;
		prog.setValue(fileIndex);
	}
	return (fileIndex == totalFiles);
}

/*! \brief Renames all files whose names don't end with .new in the input dirPath or beneath it to their name + .old.
*/
bool renameNonNewFiles(QString dirPath)
{
	QDir dir(dirPath);
	QStringList filters;
	dir.setNameFilters(filters);
	dir.setFilter(QDir::Files);
	QStringList files = dir.entryList();
	foreach(QString file,files)
	{
		//If the directory already has this file+.old and it can't be removed, give up.
		if(dir.exists(file+".old") && !dir.remove(file+".old"))
			return false;
		//If the file doesn't end with .new and it can't be renamed to file+.old, give up.
		if(!QDir::match("*.new",file) && !dir.rename(file,file+".old"))
			return false;
		//If we got here, the file either ends with .new or was renamed to file+.old
	}

	dir.setNameFilters(QStringList());
	dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	QFileInfoList dirKids = dir.entryInfoList();
	foreach(QFileInfo dirKid,dirKids)
	{
		if(!renameNonNewFiles(dirKid.absoluteFilePath()))
			return false;
		if(!QDir::match("*.new",dirKid.fileName()) && !dir.rename(dirKid.fileName(),dirKid.fileName()+".old"))
			return false;
	}
	return true;
}

/*! \brief Removes the input suffix from all files in or beneath the input directory path.
*/
bool remove4LetterSuffixOnFiles(QString dirPath,QString suffix)
{
	Q_ASSERT(suffix.length() == 4);
	QDir dir(dirPath);
	QStringList filters;
	filters << "*"+suffix;
	dir.setNameFilters(filters);
	dir.setFilter(QDir::Files);
	QStringList files = dir.entryList();
	foreach(QString file,files)
	{
		QString newName = file;
		newName.chop(4);
		//If a file with the name we want to rename to exists and cant be removed, give up.
		if(dir.exists(newName) && !dir.remove(newName))
			return false;
		//If we cant rename the file to lost the suffix, give up.
		if(!dir.rename(file,newName))
			return false;
	}

	dir.setNameFilters(QStringList());
	dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	QFileInfoList dirKids = dir.entryInfoList();
	foreach(QFileInfo dirKid,dirKids)
	{
		if(!remove4LetterSuffixOnFiles(dirKid.absoluteFilePath(),suffix))
			return false;
	}
	
	//Rename directories with the name suffix
	dir.setNameFilters(QStringList() << "*"+suffix);
	dirKids = dir.entryInfoList();
	foreach(QFileInfo dirKid,dirKids)
	{
		QString newName = dirKid.fileName();
		newName.chop(4);
		if(dir.exists(newName) && !dir.remove(newName))
			return false;
		if(!dir.rename(dirKid.fileName(),newName))
			return false;
	}

	return true;
}

/*! \brief Finishes the process of updating application files and restarts the application.
 *	\details Appends ".old" suffixes to everything in the application directory that isn't ".new", removes all ".new" suffixes from newly
 *	downloaded application files, starts a new instance of the application in a new process and exits the current application process.
 */
bool UpdateDownloader::updateApps()
{
	//Append ".old" to old files and folders
	if(!renameNonNewFiles(QCoreApplication::applicationDirPath()))
		return false;
	//Remove ".new" suffix from new files and folders
	if(!remove4LetterSuffixOnFiles(QCoreApplication::applicationDirPath(),".new"))
	{
		if(!remove4LetterSuffixOnFiles(QCoreApplication::applicationDirPath(),".old"))
		{
			//INFORM THAT INSTALLATION IS CORRUPT STATE. REINSTALL.
			reportCorruptState();
		}
		return false;
	}
	
	//Start application in new process.
	bool started = true;
	if(!QProcess::startDetached(exePath_,exeArgs_))
	{
		started = false;
	}

	//Close this application
	emit closingProcess();
	if(!rootWidgetPointer_.isNull())
	{
		rootWidgetPointer_->close();
		QCoreApplication::processEvents();
	}
	if(!started)
		reportUnableToRestart();
	exit(0);
	return false;
}

/*! \brief Shows a message box with an error message indicating that Picto couldn't remove the old application files for the update.
 *	\details The MessageBox times itself out after MESSAGEBOXDELAYSEC if requireInteraction_ is not set.
 */
void UpdateDownloader::reportUnableToRemoveOldFiles()
{
	TimedMessageBox box(requireInteraction_?0:MESSAGEBOXDELAYSEC);
	box.setWindowTitle("Picto Software Update");
	box.setText("Picto cannot remove old files.								");
	box.setIcon(QMessageBox::Information);
	box.setInformativeText(	"Picto was unable to remove old files and cannot "
							"update. Please try again or update manually if "
							"this problem continues.");
	box.exec();
}

/*! \brief Shows a message box with an error message indicating that Picto couldn't download the new files for the update.
 *	\details The MessageBox times itself out after MESSAGEBOXDELAYSEC if requireInteraction_ is not set.
 */
void UpdateDownloader::reportUnableToGetFiles()
{
	TimedMessageBox box(requireInteraction_?0:MESSAGEBOXDELAYSEC);
	box.setWindowTitle("Picto Software Update");
	box.setText("Picto cannot download updated application.								");
	box.setIcon(QMessageBox::Information);
	box.setInformativeText(	"Picto was unable to download the updated application. "
							"Please try again. ");
	box.setDetailedText(	"If the problem continues, speak to your Picto server "
							"administrator.");
	box.exec();
}

/*! \brief Shows a message box with an error message indicating that Picto couldn't complete the update process.
 *	\details The MessageBox times itself out after MESSAGEBOXDELAYSEC if requireInteraction_ is not set.
 */
void UpdateDownloader::reportUnableToCompleteUpdate()
{
	TimedMessageBox box(requireInteraction_?0:MESSAGEBOXDELAYSEC);
	box.setWindowTitle("Picto Software Update");
	box.setText("Picto was unable to complete the update process. Please try again.");
	box.setIcon(QMessageBox::Information);
	box.exec();
}

/*! \brief Shows a message box with an error message indicating that Picto has entered a corrupt state.  New files couldn't get properly named and then
 *	the old application files couldn't be rolled back.
 *	\details The MessageBox times itself out after MESSAGEBOXDELAYSEC if requireInteraction_ is not set.
 */
void UpdateDownloader::reportCorruptState()
{
	TimedMessageBox box(requireInteraction_?0:MESSAGEBOXDELAYSEC);
	box.setWindowTitle("Picto Software Update");
	box.setText("The Picto installation is in a corrupt state. Please manually reinstall.");
	box.setIcon(QMessageBox::Critical);
	box.exec();
}

/*! \brief Shows a message box with an error message indicating that Picto couldn't automatically restart for the update.
 *	\details The MessageBox times itself out after MESSAGEBOXDELAYSEC if requireInteraction_ is not set.
 */
void UpdateDownloader::reportUnableToRestart()
{
	TimedMessageBox box(requireInteraction_?0:MESSAGEBOXDELAYSEC);
	box.setWindowTitle("Picto Software Update");
	box.setText("Could not start updated Picto application automatically.  Please restart manually.");
	box.setIcon(QMessageBox::Information);
	box.exec();
}

/*! \brief Shows a message box requesting permission to automatically update the application.
 *	\details The MessageBox times itself out after MESSAGEBOXDELAYSEC if requireInteraction_ is not set.  In that
 *	case permission is implicitely granted.
 */
bool UpdateDownloader::acceptUpdate()
{
	TimedMessageBox box(requireInteraction_?0:MESSAGEBOXDELAYSEC);
	box.setWindowTitle("Picto Software Update");
	box.setText(		   "Picto software update available                              ");
	box.setInformativeText(	"Update now?");
	box.setDetailedText(	"A new version of Picto is available. You may continue to use "
							"your current version to design, test and analyze experiments "
							"but you will need to update in order to connect to a remote "
							"session.\n"
							);
	box.setIcon(QMessageBox::Question);
	box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	box.setDefaultButton(QMessageBox::Ok);
	return box.exec() == QMessageBox::Ok; 
}

/*! \brief Called when the server is discovered with autoCheckForUpdates().  Connects to the server to check for updated code.
 */
void UpdateDownloader::serverDiscovered(QHostAddress pictoServerAddress, quint16)
{
	serverAddress_ = pictoServerAddress;
	appUpdatePort_ = Picto::portNums->getUpdatePort();
	if(!updateCheckSocket_)
	{
		updateCheckSocket_ = QSharedPointer<QTcpSocket>(new QTcpSocket());
		connect(updateCheckSocket_.data(),SIGNAL(connected()),this,SLOT(requestServerPictoVersion()));
		connect(updateCheckSocket_.data(),SIGNAL(readyRead()),this,SLOT(serverVersionResponseReceived()));
	}
	if(updateCheckSocket_->state() != QAbstractSocket::ConnectedState)
	{
		serverVersionRequestTimer_.setInterval(5000); //In case we never connect
		serverVersionRequestTimer_.setSingleShot(true);
		serverVersionRequestTimer_.start();
		updateCheckSocket_->connectToHost(serverAddress_, appUpdatePort_, QIODevice::ReadWrite);
	}
	else
		requestServerPictoVersion();
}

/*! \brief Requests the version of Picto currently being used by the Picto Server.*/
void UpdateDownloader::requestServerPictoVersion()
{
	serverVersionRequestTimer_.setInterval(5000);	//In case we never get a response
	serverVersionRequestTimer_.setSingleShot(true);
	serverVersionRequestTimer_.start();
	if(!updateCheckSocket_ || (updateCheckSocket_->state() != QAbstractSocket::ConnectedState))
		return;

	Picto::ProtocolCommand command(QString("GETFILE %1 UPDATEPICTO/1.0").arg(INT_MAX));
	QString commandId = QString::number(++lastCommandId_);
	command.setFieldValue("Command-ID",commandId);
	command.write(updateCheckSocket_.data());
}

/*! \brief Called when a response is received indicating the version of Picto currently being used by the Picto Server.
 *	If the version is higher that this applications version, newVersionAvailable() is called, otherwise noNewVersionAvailable() is called.
 *	\sa autoCheckForUpdates()
*/
void UpdateDownloader::serverVersionResponseReceived()
{
	serverVersionRequestTimer_.stop();
	//Get response
	Picto::ProtocolResponse response;
	int r = response.read(updateCheckSocket_.data(),0);
	if(	r < 0 )
	{
	}
	else if(response.getResponseCode() != 200 && response.getResponseCode() != 404)
	{
	}
	else if(response.getFieldValue("PictoVersion").isEmpty())
	{

	}
	else if(response.getFieldValue("PictoVersion") <= PICTOVERSION)
	{
	}
	else
	{
		newVersionAvailable();
		return;
	}
	noNewVersionAvailable();
}

/*! \brief Called when Picto Server indicated that no new version is available.  Resets timer to check for a new version again according to autoCheckFreq_.
*	\sa autoCheckForUpdates()
*/
void UpdateDownloader::noNewVersionAvailable()
{
	//No updates.  Check again in autoCheckFreq_ - timeSinceLastCheck seconds.
	int secsSinceStarted = lastAutoCheckStart_.secsTo(QDateTime::currentDateTime());
	int newInterval = (autoCheckFreq_-secsSinceStarted)*1000;
	if(newInterval < 0)
		newInterval = 0;
	reattemptDiscoveryTimer_.setInterval(newInterval);
	reattemptDiscoveryTimer_.setSingleShot(true);
	reattemptDiscoveryTimer_.start();
}

/*! \brief Called when Picto Server indicated that a new version is available.  Uses update() to try to update the application.
 *	\sa autoCheckForUpdates()
*/
void UpdateDownloader::newVersionAvailable()
{
	//If we got here, there's an update
	if(stopAutoUpdate_)
		return;

	//If we got here, we should attempt update.
	bool success = update(serverAddress_.toString(),appUpdatePort_);
	if(!success)
		reattemptDiscovery();	//Update failed, reattempt.
}

/*! \brief Calls autoCheckForUpdates() unless stopAutoUpdate_ is set.
*/
void UpdateDownloader::reattemptDiscovery()
{
	if(stopAutoUpdate_)
		return;
	autoCheckForUpdates(autoCheckFreq_);
}