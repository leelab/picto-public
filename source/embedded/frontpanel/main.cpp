/*! \file frontpanel/main.cpp
 *	\brief This application controls the front panel on PictoBox
 *
 *	The front panel on PictoBox consists of a 2 line LCD and a rotary encoder
 *	plus button input device (a PhidgetEncoder, part number 1052).  The application
 *	handles user input and communicates with the Picto engine via a simple TCP/IP
 *	connection.  
 *	
 */

#include <QApplication>
#include <QTimer>
#include <QTcpServer>

#include <QTextStream>

#include "../../common/common.h"
#include "../../common/globals.h"
#include "../../common/namedefs.h"

#include "legacyphidgets.h"
#include "phidgets.h"

#include "menu.h"
#include "engineconnections.h"
#include "FrontPanelInfo.h"


/*! \brief The main method for the Picto Front Panel application.  
 * \details This function initializes various Picto libraries, creates a Phidgets object
 * which supplied the input from the Phidgets knob and an interface to the output LCD display
 * and creates the Front Panel objects that handle the display and IO logic (Menu) and the
 * connection between the front panel and the Picto Director application (FrontPanelInfo,
 * EngineConnections), then it starts the application running
 */
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

	//Picto applications only communicate with other picto applications that have the same
	//system number.  This is a patch that is used until someone has more time to optimize 
	//the picto server such that it can handle multiple Picto systems simultaneously
	//Here, we check if we need to reset our system number, and if so we do it
	int sysNumArgIdx = app.arguments().indexOf("-systemNumber");
	if(sysNumArgIdx > 0)
	{
		QString systemNumber = app.arguments()[sysNumArgIdx+1];
		//Change System number if necessary
		Picto::portNums->setSystemNumber(app.applicationFilePath(),app.arguments(),systemNumber.toInt(),false);
	}

	int legPhidgets = 1;
	int legArgIdx = app.arguments().indexOf("-legacyPhidgets");
	if (legArgIdx > 0)
	{
		legPhidgets = (app.arguments()[legArgIdx + 1]).toInt();
	}


	QTextStream outstream(stdout);
	outstream<<"PictoBox Front Panel Display Activated\n";
	outstream.flush();

	FrontPanelInfo *panelInfo = new FrontPanelInfo();
	
	int retVal;

//#ifdef LEGACY_PHIDGETS
	if (legPhidgets == 1)
	{
		LegacyPhidgets phidgets(panelInfo);
		Menu M(panelInfo);
		EngineConnections E(panelInfo);

		//Connect the phidgets API signals to the Menu object in the Picto Front Panel Application so 
		//that the phidgets LCD and knob can handle output and input
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

		retVal = app.exec();
		phidgets.turnOffBacklight();
		phidgets.updateLCD(1, "");
		phidgets.updateLCD(2, "");
	}
//#else
	else
	{
		Phidgets phidgets(panelInfo);
		Menu M(panelInfo);
		EngineConnections E(panelInfo);

		//Connect the phidgets API signals to the Menu object in the Picto Front Panel Application so 
		//that the phidgets LCD and knob can handle output and input
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

		retVal = app.exec();
		phidgets.turnOffBacklight();
		phidgets.updateLCD(1, "");
		phidgets.updateLCD(2, "");
		//#endif
	}
	Picto::CloseLib();
	return retVal;
}

