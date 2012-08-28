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
#include "../archives/archives.h"
#include "../protocol/protocolresponse.h"
#include "../network/serverdiscoverer.h"

using namespace Picto;

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