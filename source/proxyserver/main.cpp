/*! \file proxyserver/main.cpp
 * \brief A proxy application used to gather data from a Neural Data acquisition device (Plexon, TDT,etc) and send it 
 *	to the PictoServer.
 *
 *	\details Picto needs to be able to collect and align data produced by neural data acquisition
 *	devices.  These include Plexon, TDT and many other devices.  To collect this data, a
 *	proxy application is used.  The proxy application runs on a machine that has access to the data 
 *	(most devices have a dedicated PC that is used for spike sorting, so the proxy would
 *	run there).  During an active session, the Proxy sends the latest gathered neural data to the 
 *	PictoServer in real time.
 *
 *	Since there are so many different neural acquisition devices, the proxy was written with a plugin 
 *	architecture.  Each plugin is tailored to a specific device or in one case a "Virtual" device.
 *	The plugin architecture was written with advice from the second edition of "C++ GUI
 *	Programming with Qt" (Chapter 21).
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QLibrary>
#include <QtWidgets>
#include <QLocale>
#include <QCoreApplication>

#include "../common/common.h"
#include "../common/globals.h"
#include "../common/namedefs.h"
#include "../common/update/updatedownloader.h"
#include "proxymainwindow.h"
#include "NeuralDataAcqInterface.h"


#ifdef Q_OS_MAC
#include <sys/types.h>
#include <unistd.h>
#endif
#include "../common/mainmemleakdetect.h"

/*! \brief The main method of the Picto Proxy application.
 *	\details Goes through standard Picto application setup procedure, setting up the QApplication,
 *	the QTranslator, and calling Picto::InitializeLib() and Picto::IniitializePorts(), then sets up
 *	the AutoUpdater system with the commands that will be needed to restart this application and its top
 *	level window (so that it can correctly close the window during restart). Next a new ProxyMainWindow is
 *	displayed and the application is executed by calling QApplication::exec(), which only returns when
 *	the main window is closed.
 */
int main(int argc, char *argv[])
{
	//This will cause memory leaks to print out on exit if they're enabled
	#if defined (DETECTMEMLEAKS) && defined(_MSC_VER) && defined(_DEBUG)
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
	
	//Add plugins directory to the path
	QApplication app(argc,argv);
	QApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	QTranslator appTranslator;
	appTranslator.load(":/translations/server_" + localeLanguageCode + ".qm");
	app.installTranslator(&appTranslator);

	Picto::InitializeLib(&app,localeLanguageCode);
	Picto::InitializePorts(Picto::Names->proxyServerAppName);

	ProxyMainWindow window;

	//Setup autoupdate system
	UpdateDownloader::getInstance()->setRootWidget(&window);
	UpdateDownloader::getInstance()->setRestartCommands(app.applicationFilePath(),app.arguments());

	QIcon icon;

	icon.addFile(":/common/images/scope.png");

	app.setWindowIcon(icon);

	window.show();
	int retVal = app.exec();
	Picto::CloseLib();
	return retVal;

}