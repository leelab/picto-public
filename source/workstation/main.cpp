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

#include "mainwindow.h"

int main(int argc, char *argv[])
{
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

	mainWin.resize(1450,1024);
	mainWin.show();

	int result = app.exec();

	Picto::CloseLib();

	return(result);
}
