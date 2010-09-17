/*! \file frontpanel/main.cpp
 *	\brief This application controls the front panle on PictoBox
 *
 *	The front panel on PictoBox consists of a 2 line LCD and a rotary encoder
 *	plus button input device (a PhidgetEncoder, part number 1052).  The application
 *	handles user input and communicates with the Picto engine via a simple TCP/IP
 *	connection.  
 *	
 *	It should be noted that as of August 31, 2010, the current incarnation of
 *	PictoDirector does not interact with the FrontPanel at all.  If you want to
 *	see how this functionality should eventually be implemented, check out 
 *	embedded/dumbEngine.
 */

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
