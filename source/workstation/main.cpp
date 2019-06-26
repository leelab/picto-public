/*!	\file workstation/main.cpp
 *	\brief The Workstation application
 *
 *	Picto workstation is the operator's interface into the Picto System.  It handles everything
 *	that the operator could possibly want to do.  It's features include:
 *	- A StateMachineEditor - A graphical state machine based development environment that is used to design Picto Experiments and their Analysis.
 *	- A TestViewer - A test environment where Experiments and Analyses can be run and debugged before being deployed in a real experimental Session.
 *	- A RemoteViewer - A control panel that is used to start an experimental session on a Director/Proxy system, view it in real time, control
 *			its operation, and interact with the experiment subject.
 *	- A ReplayViewer - A playback system that allows for replaying completed experimental Sessions, recording their activity to video, running
 *		Analysis on the Sessions, and saving the Analysis output to disk. 
 *	
 *	While each of these Workstation "Viewers" functions to a great extent in its own world, there are some loose ties between all Viewers.  The main
 *	tie between all of these viewers is a "current design file."  The Workstation as whole always has a design file loaded.  The Design stored in the
 *	design file's XML has different functions in each Viewer.  In the StateMachineEditor, the design is what is being created and edited.  In the 
 *	TestViewer the design is what is being run and tested.  In the RemoteViewer the design is what is being sent over to a Director in order to run
 *	an experimental Session.  In the ReplayViewer, the design defines the Analyses that can be imported into the currently loaded Session files
 *	in order to gather data that may not have been considered when a Session was first run.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#include <QApplication>
#include <QLabel>
#include <QFont>
#include <QTextCodec>
#include <QObject>
#include <QLocale>
#include <QTranslator>

#include "../common/common.h"
#include "../common/globals.h"
#include "../common/namedefs.h"
#include "../common/update/updatedownloader.h"

#include "mainwindow.h"
#include "../common/mainmemleakdetect.h"

#if WIN32
#include <QSettings>
#include <QDir>
#include <QtWinExtras>
#endif

/*! \brief The main method of the Picto Workstation application.
 *	\details Goes through standard Picto application setup procedure, setting up the QApplication,
 *	the QTranslator, and calling Picto::InitializeLib() and Picto::IniitializePorts(), then sets up
 *	the AutoUpdater (UpdateDownloader) system with the commands that will be needed to restart this application and its top
 *	level window (so that it can correctly close the window during restart). Next the top level MainWindow is
 *	displayed and the application is executed by calling QApplication::exec(), which only returns when
 *	the MainWindow is closed.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
int main(int argc, char *argv[])
{
	//This will cause memory leaks to print out on exit if they're enabled
	#if defined (DETECTMEMLEAKS) && defined(_MSC_VER) && defined(_DEBUG)
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

    Q_INIT_RESOURCE(workstation);

	QApplication app(argc, argv);

#if WIN32
	//Register the application with its compatible filetypes
	QString exeFileName = QCoreApplication::applicationFilePath();
	exeFileName = exeFileName.right(exeFileName.length() - exeFileName.lastIndexOf("/") - 1);
	QString appName = "PictoWorkstation";

	QSettings regApplications("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\" + exeFileName, QSettings::NativeFormat);
	regApplications.setValue("FriendlyAppName", appName);

	regApplications.beginGroup("SupportedTypes");
	regApplications.setValue(".xml", QString());
	regApplications.setValue(".sqlite", QString());
	regApplications.endGroup();

	regApplications.beginGroup("shell");
	regApplications.beginGroup("open");
	regApplications.setValue("FriendlyAppName", appName);
	regApplications.beginGroup("command");
	regApplications.setValue(".", '"' + QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + "\" \"%1\"");
	regApplications.endGroup();
	regApplications.endGroup();
	regApplications.endGroup();

	//Create the Jumplist for easy opening of recent files
	QWinJumpList jumplist;
	jumplist.recent()->setTitle("Recent Experiments");
	jumplist.recent()->setVisible(true);
#endif
	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	QTranslator appTranslator;
	appTranslator.load(":/translations/server_" + localeLanguageCode + ".qm");
	app.installTranslator(&appTranslator);

	Picto::InitializeLib(&app,localeLanguageCode);
	Picto::InitializePorts(Picto::Names->workstationAppName);

	//If there is a command of -bgColor use it to figure out the version of phidgets api to call
	int bgColRArgIdx = app.arguments().indexOf("-R");
	int bgColR = 0;
	if (bgColRArgIdx > 0)
	{
		bgColR = app.arguments()[bgColRArgIdx + 1].toInt();
	}

	//If there is a command of -bgColor use it to figure out the version of phidgets api to call
	int bgColGArgIdx = app.arguments().indexOf("-G");
	int bgColG = 0;
	if (bgColGArgIdx > 0)
	{
		bgColG = app.arguments()[bgColGArgIdx + 1].toInt();
	}

	//If there is a command of -bgColor use it to figure out the version of phidgets api to call
	int bgColBArgIdx = app.arguments().indexOf("-B");
	int bgColB = 0;
	if (bgColBArgIdx > 0)
	{
		bgColB = app.arguments()[bgColBArgIdx + 1].toInt();
	}

	QColor bgColor(bgColR, bgColG, bgColB, 255);

	MainWindow mainWin(bgColor);
	UpdateDownloader::getInstance()->setRootWidget(&mainWin);
	UpdateDownloader::getInstance()->enableUpdateWarning(true);
	UpdateDownloader::getInstance()->setRestartCommands(app.applicationFilePath(),app.arguments());
	UpdateDownloader::getInstance()->requireUserInteraction(true);
	

	//mainWin.resize(1450,1024);
	mainWin.show();
	//Start automatic update checks.
	UpdateDownloader::getInstance()->autoCheckForUpdates(9000);	//Check every 15 minutes
	int result = app.exec();

	Picto::CloseLib();

	return(result);
}
