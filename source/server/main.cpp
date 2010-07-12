/*! \file main.cpp
 * \ingroup pictoserver
 * \brief
 */

#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QLibrary>
#include <QtGui>
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
#include "service/systemservice.h"
#include "dialog.h"
#include "InteractiveSTDIOHandler.h"
#include "datacollection/neuraldatacollector.h"
#include "datacollection/alignmenttool.h"
#include "connections/ServerConfig.h"

#ifdef Q_WS_MAC
#include <sys/types.h>
#include <unistd.h>

#include "processinfo/GetPID.h"
#endif

#ifdef Q_WS_WIN
#include "processinfo/WinGetPID.h"
#endif

/*! \todo we need to set up a database structure for the server.  This should consist of:
 *        a configuration database tracking server settings (IP to listen on, ports, etcetera)
 *        a session database tracking current connections, running experiments, observers, etcetera
 *          this should likely be an in-memory database accessed via an object coordinating concurrent
 *          access amongst threads.  while this is possible with SQLite, it isn't possible with QSqlDatabase
 *          connections; so this in-memory object may need to simply store this in a non SQL data structure
 *        individual experiment databases
 *          these will need to be written to sparingly (likely during the ITI) due to atomic commits to disk
 *          being somewhat slow.  So as to handle network distribution of the data, we can maintain in-memory
 *          trial representations that are flushed, emit signals when new data is present, and allow read-only
 *          threaded access to the object
 *        a master database of all experiments
 */

int serviceMain(SystemService *)
{
	QEventLoop eventLoop;

	QSharedPointer<ServerProtocols> httpProtocols(new ServerProtocols());
	QSharedPointer<ServerProtocols> pictoProtocols(new ServerProtocols());

	QSharedPointer<ServerHTTPProtocol> httpProtocol(new ServerHTTPProtocol());
	QSharedPointer<ServerPictoProtocol> pictoProtocol(new ServerPictoProtocol());
	
	httpProtocols->addProtocol(httpProtocol);
	pictoProtocols->addProtocol(httpProtocol);
	pictoProtocols->addProtocol(pictoProtocol);

	/*! \todo we need to check for the presence of a session database, and if present handle a crashed session
	 *        recovery.  Initially, that recovery may be non-existent (i.e. we'll just delete the old session
	 *        database).
	 *        This should also all be abstracted into a seperate object layer.
	 */
	ServerConfig config;
	config.clearProxyServers();
	

	/*! \todo this should specify the IP address in addition to the port, and both should be read from the
	 *        configuration database.
	 */
	Server httpServer(80, httpProtocols);
	Server pictoServer(42424, pictoProtocols);


	return eventLoop.exec();
}

int main(int argc, char *argv[])
{
	bool bUseGUI = false;

	//If we have no parameters, and a windowing system is available, we'll put up a GUI
#ifdef Q_WS_MAC
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
#ifdef Q_WS_X11
		//On Unix OSes using an X11 display manager, we'll simply look for the DISPLAY
		//environment variable.  If it's there, we'll put up a GUI; otherwise, we'll use
		//the terminal so as to allow remote usage via SSH.
		bUseGUI = getenv("DISPLAY") != 0;
#elif defined Q_WS_MAC
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
#elif defined Q_WS_WIN
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
												 "Starting %1 in interactive mode.\n").
													arg(Picto::Names->serverAppName);
					outputStream << "Type \"quit\" on a single line to exit\n";
					outputStream.flush();

					InteractiveSTDIOHandler interactiveSTDIOHandler;
					interactiveSTDIOHandler.start();

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

/*! \defgroup pictoserver Picto Server
 *  \defgroup pictoserver_network Network
 *		\ingroup pictoserver
 *  \defgroup pictoserver_processinfo ProcessInfo
 *		\ingroup pictoserver
 *  \defgroup pictoserver_protocol Protocol
 *		\ingroup pictoserver
 *  \defgroup pictoserver_service Service
 *		\ingroup pictoserver
 */