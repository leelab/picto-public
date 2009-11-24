#include <QApplication>
#include <QHostAddress>

#include <QTextStream>

#include "../../common/common.h"
#include "../../common/globals.h"
#include "../../common/namedefs.h"

#include "engine.h"


int main(int argc, char *argv[])
{
	QApplication::setDesktopSettingsAware(false);
	QApplication app(argc, argv);

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	Picto::InitializeLib(&app,localeLanguageCode);

	QTextStream outstream(stdout);
	outstream<<"Hello world\n";
	outstream.flush();

	Engine eng("MattBox", QHostAddress("9.250.54.32"));
	eng.runExperiment(3,200);

	bool result = app.exec();

	Picto::CloseLib();

	return result;
}
