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

QSharedPointer<UpdateDownloader> UpdateDownloader::getInstance()
{
	if(singleton_.isNull())
	{
		singleton_ = QSharedPointer<UpdateDownloader>(new UpdateDownloader());
	}
	return singleton_;
}

void UpdateDownloader::setRootWidget(QWidget* root)
{
	rootWidgetPointer_ = root;
}

void UpdateDownloader::enableUpdateWarning(bool en)
{
	useWarning_ = en;
}

void UpdateDownloader::requireUserInteraction(bool en)
{
	requireInteraction_ = en;
}

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
	return true;
}

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

void UpdateDownloader::setRestartCommands(QString exePath,QStringList args)
{
	exePath_ = exePath;
	exeArgs_ = args;
}

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
	if(!updateSocket_->waitForConnected(20000))
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
		QByteArray fileData = response.getDecodedContent();
		
		//If the directory for this file doesn't exist, create it in bin
		if(!dir.isEmpty() && !binDir.exists(dir))
		{	//If the directory doesn't exist, make it
			if(!binDir.mkdir(dir))
				break;
		}
		
		//Create the file in bin.new or its appropriate subdirectory
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

bool renameNonNewFiles(QString dirPath)
{
	QDir dir(dirPath);
	QStringList filters;
	dir.setNameFilters(filters);
	dir.setFilter(QDir::Files);
	QStringList files = dir.entryList();
	foreach(QString file,files)
	{
		if(dir.exists(file+".old") && !dir.remove(file+".old"))
			return false;
		if(!QDir::match("*.new",file) && !dir.rename(file,file+".old"))
			return false;
	}

	dir.setNameFilters(QStringList());
	dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	QFileInfoList dirKids = dir.entryInfoList();
	foreach(QFileInfo dirKid,dirKids)
	{
		if(!renameNonNewFiles(dirKid.absoluteFilePath()))
			return false;
	}
	return true;
}

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
		if(dir.exists(newName) && !dir.remove(newName))
			return false;
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
	return true;
}

bool UpdateDownloader::updateApps()
{
	//Append ".old" to old files
	if(!renameNonNewFiles(QCoreApplication::applicationDirPath()))
		return false;
	//Remove ".new" suffix from new files
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

void UpdateDownloader::reportUnableToCompleteUpdate()
{
	TimedMessageBox box(requireInteraction_?0:MESSAGEBOXDELAYSEC);
	box.setWindowTitle("Picto Software Update");
	box.setText("Picto was unable to complete the update process. Please try again.");
	box.setIcon(QMessageBox::Information);
	box.exec();
}

void UpdateDownloader::reportCorruptState()
{
	TimedMessageBox box(requireInteraction_?0:MESSAGEBOXDELAYSEC);
	box.setWindowTitle("Picto Software Update");
	box.setText("The Picto installation is in a corrupt state. Please manually reinstall.");
	box.setIcon(QMessageBox::Critical);
	box.exec();
}

void UpdateDownloader::reportUnableToRestart()
{
	TimedMessageBox box(requireInteraction_?0:MESSAGEBOXDELAYSEC);
	box.setWindowTitle("Picto Software Update");
	box.setText("Could not start updated Picto application automatically.  Please restart manually.");
	box.setIcon(QMessageBox::Information);
	box.exec();
}

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

void UpdateDownloader::serverDiscovered(QHostAddress pictoServerAddress, quint16)
{
	serverAddress_ = pictoServerAddress;
	appUpdatePort_ = APPUPDATEPORT;
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
	else if(response.getFieldValue("PictoVersion") == PICTOVERSION)
	{
	}
	else
	{
		newVersionAvailable();
		return;
	}
	noNewVersionAvailable();
}

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

void UpdateDownloader::reattemptDiscovery()
{
	if(stopAutoUpdate_)
		return;
	autoCheckForUpdates(autoCheckFreq_);
}