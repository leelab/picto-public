/*! \file proxyserver/main.cpp
 * \brief A proxy server used to send data from the Data acquisition devices (Plexon, TDT,etc) to PictoServer
 *
 *	Picto needs to be able to collect and organize data produced by neural data acquisition
 *	devices.  These include Plexon, TDT, and many other devices.  To collect this data, a
 *	proxy server is used.  the proxy server runs on a machine that has access to the data 
 *	(most devices have a dedicated PC that is used for spike sorting, so the proxy would
 *	run there).  During an active session, PictoServer periodically requests the latest 
 *	neural data from the proxy server using the ACQ protocol.
 *
 *	Since there are so many different neural acquisition devices, the proxy server was
 *	written with a plugin architecture.  Each plugin is tailored to a specific device.
 *	The plugin architecture was written with advice from the second edition of "C++ GUI
 *	Programming with Qt" (Chapter 21).
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
#include "../common/archives/archives.h"
#include "../common/update/updatedownloader.h"
#include "network/proxyserver.h"
#include "protocol/ProxyServerProtocols.h"
#include "protocol/ProxyServerAcqProtocol.h"
#include "InteractiveSTDIOHandler.h"
#include "proxymainwindow.h"
#include "NeuralDataAcqInterface.h"


#ifdef Q_OS_MAC
#include <sys/types.h>
#include <unistd.h>
#endif
#include "../common/mainmemleakdetect.h"

int main(int argc, char *argv[])
{
	//This will cause memory leaks to print out on exit if they're enabled
	#if defined (DETECTMEMLEAKS) && defined(_MSC_VER) && defined(_DEBUG)
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
	
	//Add plugins directory to the path
	QApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");
	QApplication app(argc,argv);

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	QTranslator appTranslator;
	appTranslator.load(":/translations/server_" + localeLanguageCode + ".qm");
	app.installTranslator(&appTranslator);

	Picto::InitializeLib(&app,localeLanguageCode);

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

/*! \defgroup pictoproxyserver Picto Server
 *  \defgroup pictoproxyserver_network Network
 *		\ingroup pictoproxyserver
 *  \defgroup pictoproxyserver_processinfo ProcessInfo
 *		\ingroup pictoproxyserver
 *  \defgroup pictoproxyserver_protocol Protocol
 *		\ingroup pictoproxyserver
 *  \defgroup pictoproxyserver_service Service
 *		\ingroup pictoproxyserver
 */