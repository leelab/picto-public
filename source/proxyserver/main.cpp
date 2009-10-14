/*! \file main.cpp
 * \ingroup pictoproxyserver
 * \brief A proxy server used to send data from the Data acquisition devices (Plexon, TDT,etc) to PictoServer
 */

#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QLibrary>
#include <QtGui>
#include <QLocale>
#include <QCoreApplication>

#include "../common/common.h"
#include "../common/globals.h"
#include "../common/namedefs.h"
#include "../common/archives/archives.h"
#include "network/server.h"
#include "protocol/ServerProtocols.h"
#include "protocol/ServerAcqProtocol.h"
#include "protocol/ServerHTTPProtocol.h"
#include "InteractiveSTDIOHandler.h"
#include "mainwindow.h"
#include "interfaces.h"

#ifdef Q_WS_MAC
#include <sys/types.h>
#include <unistd.h>

#include "processinfo/GetPID.h"
#endif

#ifdef Q_WS_WIN
#include "processinfo/WinGetPID.h"
#endif


int main(int argc, char *argv[])
{
	QApplication app(argc,argv);

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	QTranslator appTranslator;
	appTranslator.load(":/translations/server_" + localeLanguageCode + ".qm");
	app.installTranslator(&appTranslator);

	Picto::InitializeLib(&app,localeLanguageCode);

	
	MainWindow window;

	QIcon icon;

	icon.addFile(":/common/images/scope.png");

	app.setWindowIcon(icon);

	window.show();
	return app.exec();

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