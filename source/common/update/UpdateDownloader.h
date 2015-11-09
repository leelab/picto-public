#ifndef _UPDATEDOWNLOADER_H_
#define _UPDATEDOWNLOADER_H_

#include <QApplication>
#include <QObject>
#include <QSharedPointer>
#include <QFile>
#include <QTcpSocket>
#include <QHostAddress>
#include <QProgressBar>
#include <QPointer>
#include <QDateTime>

#ifdef WINCE
#include <winbase.h>
#endif
#include "../common.h"
#include "../globals.h"
#include "../namedefs.h"
#include "../protocol/protocolresponse.h"
#include "../network/serverdiscoverer.h"

using namespace Picto;

/*! \brief A singleton class used to check for and automatically update to a newer version of the Picto Application.
 *
 *	\details Currently, we put all Picto application files into all Picto installation directories regardless of which of
 *	the Picto applications will actually be run on a particular computer.  This means that the PictoServer has a copy of
 *	all Picto Application files needed by all Picto computers.  Whenever the PictoServer sends a message, it reports its
 *	current Picto Version.  If the client sees that its version is lower than the server's version, it can request newer
 *	versions of the application files by using this class' object.  The server keeps a cached copy of the zipped files and
 *	sends them over the network to the object.  The object then goes through some fancy footwork to carefully upgrade the
 *	application in a reversible way.  It first puts all of the new files into the application directory with .new suffixes
 *	appended to their names.  It then tries to rename all of the other files in the application directory and add .old to
 *	their names.  If this all works correctly, the indication is that there should be no problem renaming all the .new
 *	files by removing their .new suffixes.  If that works, the application starts a new application process and ends the
 *	current one, thereby completing the update.  If anything about the update fails, the application rolls all of the
 *	application files back to their previous condition.  Since all files with .old or .new suffixes are always deleted at
 *	the beginning of the update process (before files are even downloaded) we don't have to worry about corruption from
 *	old files.
 *
 *	This object also makes heavy use of TimedMessageBox widgets so that Automatic updating can occur on Director and Proxy
 *	installations with zero user input, completely automatically.  
 *
 *	The class is fairly stand alone.  You pretty much just need to add it to every application, call a few setup functions
 *	like setRootWidget(), and possibly requireUserInteraction(), enableUpdateWarning() and/or autoCheckForUpdates()
 *	depending on the particular application use case and let the object do the rest.
 *
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API UpdateDownloader : public QObject
#else
class UpdateDownloader : public QWidget
#endif
{
	Q_OBJECT
public:
    static QSharedPointer<UpdateDownloader> getInstance();
	void setRootWidget(QWidget* root);
	void requireUserInteraction(bool en);
	void enableUpdateWarning(bool en);
	bool update(QString serverAddress, quint16 port);
	void setRestartCommands(QString exePath,QStringList args);
	void autoCheckForUpdates(int freqSec);
signals:
	void updateFailed();
	void closingProcess();
private:
	UpdateDownloader();
	bool getFiles();
	/*! /brief Updates apps
	 *	Runs UpdatePicto.exe in a new process, and exits this process to rename the directory
	 *	with the new executables and delete the old one, then restart the application.
	 */
    bool updateApps();
	void reportUnableToRemoveOldFiles();
	void reportUnableToGetFiles();
	void reportUnableToCompleteUpdate();
	void reportCorruptState();
	void reportUnableToRestart();
	bool acceptUpdate();
	QHostAddress serverAddress_;
	quint16 appUpdatePort_;
	QSharedPointer<QTcpSocket> updateSocket_;
	QSharedPointer<QTcpSocket> updateCheckSocket_;
	QString updaterFileName_;
	QString exePath_;
	QStringList exeArgs_;
	bool useWarning_;
	bool requireInteraction_;
	bool updating_;
	QDateTime lastAutoCheckStart_;
	QTimer reattemptDiscoveryTimer_;
	QTimer serverVersionRequestTimer_;
	int autoCheckFreq_;
	bool stopAutoUpdate_;

	static int lastCommandId_;
	static QSharedPointer<UpdateDownloader> singleton_;
	QPointer<QWidget> rootWidgetPointer_;
	QSharedPointer<ServerDiscoverer> serverDiscoverer_;

private slots:
	void serverDiscovered(QHostAddress pictoServerAddress, quint16 pictoServerPort);
	void requestServerPictoVersion();
	void serverVersionResponseReceived();
	void noNewVersionAvailable();
	void newVersionAvailable();
	void reattemptDiscovery();
};
#endif