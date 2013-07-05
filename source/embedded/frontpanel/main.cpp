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
	//This will cause memory leaks to print out on exit if they're enabled
	#if defined (DETECTMEMLEAKS) && defined(_MSC_VER) && defined(_DEBUG)
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
	QApplication::setDesktopSettingsAware(false);
	QApplication app(argc, argv);

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	Picto::InitializeLib(&app,localeLanguageCode);
	Picto::InitializePorts(Picto::Names->frontPanelAppName);

	//possibly reset our system number...
	int sysNumArgIdx = app.arguments().indexOf("-systemNumber");
	if(sysNumArgIdx > 0)
	{
		QString systemNumber = app.arguments()[sysNumArgIdx+1];
		//Change System number if necessary
		Picto::portNums->setSystemNumber(app.applicationFilePath(),app.arguments(),systemNumber.toInt(),false);
	}

	QTextStream outstream(stdout);
	outstream<<"PictoBox Front Panel Display Activated\n";
	outstream.flush();

	FrontPanelInfo *panelInfo = new FrontPanelInfo();
	
	Phidgets phidgets(panelInfo);
	Menu M(panelInfo);
	EngineConnections E(panelInfo);

	QObject::connect(&M, SIGNAL(updateLCD(int, QString)), &phidgets, SLOT(updateLCD(int, QString)));
	QObject::connect(&M, SIGNAL(toggleBacklight()), &phidgets, SLOT(toggleBacklight()));
	QObject::connect(&M, SIGNAL(turnOnBacklight()), &phidgets, SLOT(turnOnBacklight()));
	QObject::connect(&M, SIGNAL(turnOffBacklight()), &phidgets, SLOT(turnOffBacklight()));
	QObject::connect(&app, SIGNAL(aboutToQuit()), &M, SLOT(aboutToQuit()));
	QObject::connect(&phidgets, SIGNAL(userInputSignal(int)), &M, SLOT(userInputSlot(int)));
	//QObject::connect(&E, SIGNAL(newEventRead()), &M, SLOT(updateStatus()));

	M.initMenu();


	//For debugging: Since CE doesn't like to force quit stuff, we'll quit after a fixed time
	/*QTimer timer;
	timer.setInterval(30000);
	QObject::connect(&timer, SIGNAL(timeout()), &app, SLOT(quit()));
	timer.start();*/
	

	int retVal = app.exec();
	phidgets.turnOffBacklight();
	phidgets.updateLCD(1,"");
	phidgets.updateLCD(2,"");
	Picto::CloseLib();
	return retVal;
}
