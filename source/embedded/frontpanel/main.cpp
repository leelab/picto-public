#include <QApplication>
#include <QTimer>
#include <QTcpServer>

#include <QTextStream>

#include "../../common/common.h"
#include "../../common/globals.h"
#include "../../common/namedefs.h"

#include "phidgets.h"
#include "menu.h"
#include "engineconnections.h"
#include "FrontPanelInfo.h"



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

	FrontPanelInfo *panelInfo = new FrontPanelInfo();
	
	Phidgets phidgets(panelInfo);
	Menu M(panelInfo);
	EngineConnections E(panelInfo);

	QObject::connect(&M, SIGNAL(updateLCD(int, QString)), &phidgets, SLOT(updateLCD(int, QString)));
	QObject::connect(&M, SIGNAL(toggleBacklight()), &phidgets, SLOT(toggleBacklight()));
	QObject::connect(&M, SIGNAL(turnOnBacklight()), &phidgets, SLOT(turnOnBacklight()));
	QObject::connect(&phidgets, SIGNAL(userInputSignal(int)), &M, SLOT(userInputSlot(int)));
	QObject::connect(&E, SIGNAL(newEventRead()), &M, SLOT(updateStatus()));

	M.initMenu();


	//For debugging: Since CE doesn't like to force quit stuff, we'll quit after a fixed time
	/*QTimer timer;
	timer.setInterval(30000);
	QObject::connect(&timer, SIGNAL(timeout()), &app, SLOT(quit()));
	timer.start();*/
	

	return app.exec();
}
