/*! \file server/main.cpp
 * \brief The PictoSErver application.
 *
 *	PictoServer is the central server that ties all of the distributed pieces of Picto
 *	together.  Like any good server, it recieves commands, parses and replies to them.
 *
 *	The server can be run in two modes.  The first mode is as a service.  It is referred to as GUI mode since
 *	the service is installed/removed/started/stopped through a GUI dialog.  Running the Server as a Service lets us 
 *	install it on a machine and completely ignore it.  It will actually even restart
 *	itself if it crashes.  The second mode is "interactive".  This gets run from a command line and provides feedback
 *	as the server is running.  While developing Picto, you will want to use interactive 
 *	mode exclusively, since it gives you some clue as to what is going on with the server.
 */
#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QLibrary>
#include <QtWidgets>
#include <QLocale>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>

#include "../common/common.h"
#include "../common/globals.h"
#include "../common/namedefs.h"
#include "../common/archives/archives.h"
#include "network/server.h"
#include "protocol/ServerProtocols.h"
#include "protocol/ServerPictoProtocol.h"
#include "protocol/ServerHTTPProtocol.h"
#include "protocol/ServerAppUpdateProtocol.h"
#include "service/systemservice.h"
#include "dialog.h"
#include "../common/mainmemleakdetect.h"

#ifdef Q_OS_MAC
#include <sys/types.h>
#include <unistd.h>

#include "processinfo/GetPID.h"
#endif

#ifdef Q_OS_WIN
#include "processinfo/WinGetPID.h"
#endif

/*! \brief This looks to me like the main method when the Picto Server runs as a service.  I didn't write it, but that's definitely what it looks like.
*/
int serviceMain(SystemService *)
{
	Picto::InitializeNames();
	Picto::InitializePorts(Picto::Names->serverAppName,true);

	QEventLoop eventLoop;

	QSharedPointer<ServerProtocols> httpProtocols(new ServerProtocols());
	QSharedPointer<ServerProtocols> pictoProtocols(new ServerProtocols());
	QSharedPointer<ServerProtocols> PictoUpdateProtocols(new ServerProtocols());


	QSharedPointer<ServerHTTPProtocol> httpProtocol(new ServerHTTPProtocol());
	QSharedPointer<ServerPictoProtocol> pictoProtocol(new ServerPictoProtocol());
	QSharedPointer<ServerAppUpdateProtocol> pictoUpdateProtocol(new ServerAppUpdateProtocol());
	httpProtocols->addProtocol(httpProtocol);
	pictoProtocols->addProtocol(httpProtocol);
	pictoProtocols->addProtocol(pictoProtocol);
	PictoUpdateProtocols->addProtocol(pictoUpdateProtocol);

	Server httpServer(80, httpProtocols);
	Server pictoServer(Picto::portNums->getServerPort(), pictoProtocols);
	Server appUpdateServer(Picto::portNums->getUpdatePort(), PictoUpdateProtocols);


	return eventLoop.exec();
}

/*! \brief The main method of the Picto Server application.  I didn't write this function, and its kind of a beast, so I'm going to just go ahead and
 *	paste the command line usage instructions:
 *	\code 
		Usage:
		PictoServer [-gui] [-interactive] [-install] [-start] [-remove] [-stop]

		  -gui          - Launch a graphical user interface for the
		                  Service/Daemon.  Must be the only argument.
		  -interactive  - Runs in interactive mode (as a regular application)
		                  instead of as a Service/Daemon.
		  -install      - Installs the Service/Daemon so that it will run in
		                  the background at system startup, even if no user
		                 is logged in.
		  -start        - Puts the Service/Daemon into the running state.
			                  This can be placed after -install to install and
		                  start the Service/Daemon immediately.
		  -remove       - Removes the Service/Daemon from the system.
		                  The Service/Daemon can no longer be started and
		                  will not run at system startup.
		  -stop         - Puts the Service/Daemon into the stopped state, but
		                  does not remove it from the system.
		                  The Service/Daemon will still run at system startup.
	\endcode
 */
int main(int argc, char *argv[])
{
	//This will cause memory leaks to print out on exit if they're enabled
	#if defined (DETECTMEMLEAKS) && defined(_MSC_VER) && defined(_DEBUG)
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	bool bUseGUI = false;

	//If we have no parameters, and a windowing system is available, we'll put up a GUI
#ifdef Q_OS_MAC
	//If we're on Mac OS X 10.2 or later and were launched from the finder, dock, or
	//terminal via the open command, then we'll have an extra argument containing the
	//process serial number in the form -psn_#_###.
	if(argc==2)
	{
		QString checkPSN = argv[1];

		if(checkPSN.left(5)=="-psn_")
		{
			bUseGUI = true;
		}
	}
#endif

	if(argc==1)
	{
#ifdef Q_OS_X11
		//On Unix OSes using an X11 display manager, we'll simply look for the DISPLAY
		//environment variable.  If it's there, we'll put up a GUI; otherwise, we'll use
		//the terminal so as to allow remote usage via SSH.
		bUseGUI = getenv("DISPLAY") != 0;
#elif defined Q_OS_MAC
		//On Mac OS X, if you explicitly invoke the BSD binary from within the app bundle
		//via a terminal, then we will not use a GUI (allowing for remote usage via SSH).
		//
		//We'll check if there is a terminal environment variable present, and if so we
		//will further check to see if we were invoked by the WindowServer to cover the
		//case where someone starts the app bundle on a pre-Mac OS X 10.2 machine from
		//a terminal using the open command
		bUseGUI = getenv("TERM") == 0;
		
		if(!bUseGUI)
		{
			pid_t ppid = getppid();

			if(ppid == GetPIDForProcessName("WindowServer"))
			{
				bUseGUI = true;
			}	
		}
#elif defined Q_OS_WIN
		//We'll find our parent process and check if it was a GUI application
		bUseGUI = winParentProcessIsGUI();

		//On Windows, to have standard output within a terminal, you mark your executable
		//as a console application.  If you aren't invoked from a terminal, the windowing
		//system will allocate a console for you.
		//
		//The alternative is to mark the executable as a GUI application, and try to
		//attach to the console of the parent process using AttachConsole((DWORD) -1)
		//(Note that you would also have to redirect stdout/stderr/stdin using freopen()).
		//This has the benefit of not initially showing a console window, but it has
		//two drawbacks: 1) stdout can't be redirected, because you're expressly overriding
		//any redirection via freopen(); and 2) since the invoking console doesn't know we
		//are a console application, it will return immediately - this disallows proper
		//chaining, enables multiple commands to target the console, and invalidates any
		//logic to format output.
		//
		//Since we opt to mark the executable as a console application, if we decided to
		//instantiate a GUI, we now need to hide our console window.
		if(bUseGUI)
		{
#ifndef WINCE		
			ShowWindow(GetConsoleWindow(),false);
#endif
		}
#else
		bUseGUI = true;
#endif
	}
	
	bool bUsedGUIArgument = false;

	if(argc == 2)
	{
		QString guiArgument = argv[1];
		if(guiArgument == "-gui")
		{
			bUseGUI = true;
			bUsedGUIArgument = true;
		}
	}
	
	QApplication app(argc,argv,bUseGUI);

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	QTranslator appTranslator;
	appTranslator.load(":/translations/server_" + localeLanguageCode + ".qm");
	app.installTranslator(&appTranslator);

	Picto::InitializeLib(&app,localeLanguageCode);
	Picto::InitializePorts(Picto::Names->serverAppName,true);

	QTextStream outputStream(stdout);
	const QString usageDescription =
		app.translate("ServerMain",
					  "Usage:\n"
					  "\n"
					  "%1 [-gui] [-interactive] [-install] [-start] [-remove] [-stop]\n"
					  "\n"
					  "  -gui          - Launch a graphical user interface for the\n"
					  "                  Service/Daemon.  Must be the only argument.\n"
					  "  -interactive  - Runs in interactive mode (as a regular application)\n"
					  "                  instead of as a Service/Daemon.\n"
					  "  -install      - Installs the Service/Daemon so that it will run in\n"
					  "                  the background at system startup, even if no user\n"
					  "                  is logged in.\n"
					  "  -start        - Puts the Service/Daemon into the running state.\n"
					  "                  This can be placed after -install to install and\n"
					  "                  start the Service/Daemon immediately.\n"
					  "  -remove       - Removes the Service/Daemon from the system.\n"
					  "                  The Service/Daemon can no longer be started and\n"
					  "                  will not run at system startup.\n"
					  "  -stop         - Puts the Service/Daemon into the stopped state, but\n"
					  "                  does not remove it from the system.\n"
					  "                  The Service/Daemon will still run at system startup.\n").
						arg(Picto::Names->serverAppName);
					   

	if(argc==1 && !bUseGUI)
	{
		outputStream << usageDescription;
		Picto::CloseLib();
		return(0);
	}

	const QString serviceDescription =
		app.translate("ServerMain",
					  "Enables data collection from %1 hardware and %2 software.  "
					  "Allows control and observation of experiments from %3 software.  "
					  "Manage this service using the %4 software.").arg(Picto::Names->directorHWName).
																    arg(Picto::Names->directorAppName).
																    arg(Picto::Names->workstationAppName).
																    arg(Picto::Names->configAppName);

	const QString serviceSwitch = "-service";

	SystemService systemService(Picto::Names->serverAppName,
								serviceSwitch,
								Picto::Names->serverAppName,
								serviceDescription,
								"",
								serviceMain,
								argc,
								argv);

	bool result=false;

	if(argc > 1 && !bUsedGUIArgument)
	{
		for(int iArgCounter = 1; iArgCounter < argc; iArgCounter++)
		{
			const QString argument = argv[iArgCounter];
			
			if(argument[0]=='-' || argument[0]=='/')
			{
				QString errorDescription;

				if(argument.mid(1)==serviceSwitch.mid(1))
				{
					result = systemService.runAsService();
				}
				else if(argument.mid(1)=="interactive")
				{
					outputStream << app.translate("ServerMain",
												 "Starting %1 in interactive mode on System Number: %2.\n").
													arg(Picto::Names->serverAppName).arg(Picto::portNums->getSystemNumber());
					outputStream << "Type \"quit\" on a single line to exit\n";
					outputStream.flush();

					//InteractiveSTDIOHandler interactiveSTDIOHandler;
					//interactiveSTDIOHandler.start();

					result = systemService.runInteractive();

					outputStream << "Shutting down...\n";
				}
				else if(argument.mid(1)=="install")
				{
					result = systemService.install(&errorDescription);
					if(result)
					{
						outputStream << app.translate("ServerMain",
													 "The %1 Service/Daemon has been installed.\n").
														arg(Picto::Names->serverAppName);
					}
					else
					{
						outputStream << app.translate("ServerMain",
													 "Unable to install the %1 Service/Daemon.\n").
														arg(Picto::Names->serverAppName);
						outputStream << "\n\n";
						outputStream << errorDescription;
					}
				}
				else if(argument.mid(1)=="remove")
				{
					result = systemService.remove(&errorDescription);
					if(result)
					{
						outputStream << app.translate("ServerMain",
													 "The %1 Service/Daemon has been removed.\n").
														arg(Picto::Names->serverAppName);
					}
					else
					{
						outputStream << app.translate("ServerMain",
													 "Unable to remove the %1 Service/Daemon.\n").
														arg(Picto::Names->serverAppName);
						outputStream << "\n\n";
						outputStream << errorDescription;
					}
				}
				else if(argument.mid(1)=="start")
				{
					result = systemService.start(&errorDescription);
					if(result)
					{
						outputStream << app.translate("ServerMain",
													 "The %1 Service/Daemon has been started.\n").
														arg(Picto::Names->serverAppName);
					}
					else
					{
						outputStream << app.translate("ServerMain",
													 "Unable to start the %1 Service/Daemon.\n").
														arg(Picto::Names->serverAppName);
						outputStream << "\n\n";
						outputStream << errorDescription;
					}
				}
				else if(argument.mid(1)=="stop")
				{
					result = systemService.stop(&errorDescription);
					if(result)
					{
						outputStream << app.translate("ServerMain",
													 "The %1 Service/Daemon has been stopped.\n").
														arg(Picto::Names->serverAppName);
					}
					else
					{
						outputStream << app.translate("ServerMain",
													 "Unable to stop the %1 Service/Daemon.\n").
														arg(Picto::Names->serverAppName);
						outputStream << "\n\n";
						outputStream << errorDescription;
					}
				}
				else
				{
					outputStream << usageDescription;
				}
			}
			else
			{
				outputStream << usageDescription;
			}
		}
	}
	else if(bUseGUI)
	{
		QApplication::setDesktopSettingsAware(false);

		QIcon icon;

		icon.addFile(":/common/images/scope.png");

		app.setWindowIcon(icon);

		Dialog dialog(&systemService);
		dialog.show();

		result = app.exec();
	}

	Picto::CloseLib();

	return(result);
}