/*!	\file workstation/main.cpp
 *	\brief The Workstation application
 *
 *	Picto workstation is the user's interface to the Picto System.  It will need to handle everything
 *	that the user could possibly want to do.  It's features will includes:
 *	- Experiment design
 *	- Experiment testing
 *	- Experiment control/observation
 *	- Data analysis (online and offline)
 *	- Access to archive data stored on the server
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
#include "../common/archives/archives.h"
#include "../common/random/mtrand.h"
#include "../common/update/updatedownloader.h"

#include "mainwindow.h"
#include "../common/mainmemleakdetect.h"

int main(int argc, char *argv[])
{
	//This will cause memory leaks to print out on exit if they're enabled
	#if defined (DETECTMEMLEAKS) && defined(_MSC_VER) && defined(_DEBUG)
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

    Q_INIT_RESOURCE(workstation);

	QApplication app(argc, argv);

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	QTranslator appTranslator;
	appTranslator.load(":/translations/server_" + localeLanguageCode + ".qm");
	app.installTranslator(&appTranslator);

	Picto::InitializeLib(&app,localeLanguageCode);

	Q_ASSERT_X(Picto::testMTRand(),"Director::main()","Random number generator failed test");

	MainWindow mainWin;
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
