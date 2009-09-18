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

/*int serviceMain(QObject *acqPlugin)
{
	QEventLoop eventLoop;

	QSharedPointer<ServerProtocols> httpProtocols(new ServerProtocols());
	QSharedPointer<ServerProtocols> acqProtocols(new ServerProtocols());

	QSharedPointer<ServerHTTPProtocol> httpProtocol(new ServerHTTPProtocol(acqPlugin));
	QSharedPointer<ServerAcqProtocol> acqProtocol(new ServerAcqProtocol(acqPlugin));
	
	httpProtocols->addProtocol(httpProtocol);
	acqProtocols->addProtocol(httpProtocol);
	acqProtocols->addProtocol(acqProtocol);*/



	/*! \todo this should specify the IP address in addition to the port, and both should be read from the
	 *        configuration database.
	 */
	/*Server httpServer(80, httpProtocols);
	Server pictoServer(42424, acqProtocols);


	return eventLoop.exec();
}*/

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

	/*QApplication app(argc,argv,false);  //console app

	QLocale systemLocale = QLocale();
	QString localeLanguageCode = systemLocale.name().left(2);

	QTranslator appTranslator;
	appTranslator.load(":/translations/server_" + localeLanguageCode + ".qm");
	app.installTranslator(&appTranslator);

	Picto::InitializeLib(&app,localeLanguageCode);

	QTextStream outputStream(stdout);

	const QString usageDescription =
		app.translate("ProxyServerMain",
					  "Usage:\n"
					  "\n"
					  "%1 [-listdevices] [-device :deviceName]\n"
					  "\n"
					  "The Proxy Server is used to provide data from a neural data\n"
					  "collection device (e.g. a Plexon) to the %2.\n\n"
					  "  -listdevices  - Lists the available plugins for devices that can\n"
					  "                  provide data with the proxy server\n"
					  "  -device       - Runs the proxy server using the listed device.\n").
						arg(Picto::Names->proxyServerAppName).
						arg(Picto::Names->serverAppName);
					   

	if(argc==1)
	{
		outputStream << usageDescription;
		Picto::CloseLib();
		return(0);
	}

	bool result = false;


	if(argc > 1)
	{
		for(int iArgCounter = 1; iArgCounter < argc; iArgCounter++)
		{
			const QString argument = argv[iArgCounter];
			
			if(argument[0]=='-' || argument[0]=='/')
			{
				QString errorDescription;

				if(argument.mid(1)=="device")
				{
					//get the device name
					iArgCounter++;
					QString deviceName = argv[iArgCounter];

					QObject *devicePlugin = NULL;

					//find the appropriate plugin
					foreach (QObject *plugin, QPluginLoader::staticInstances())
					{
						NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(plugin);
						if(iNDAcq->device() == deviceName)
							devicePlugin = plugin;
					}
					
					QDir pluginsDir = QDir(qApp->applicationDirPath());

	#if defined(Q_OS_WIN)
					if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
						pluginsDir.cdUp();
	#elif defined(Q_OS_MAC)
					if (pluginsDir.dirName() == "MacOS") {
						pluginsDir.cdUp();
						pluginsDir.cdUp();
						pluginsDir.cdUp();
					}
	#endif
					pluginsDir.cd("plugins");

					foreach (QString fileName, pluginsDir.entryList(QDir::Files)) 
					{
						QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
						QObject *plugin = loader.instance();
						if (plugin) 
						{
							NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(plugin);
							if(iNDAcq->device() == deviceName)
								devicePlugin = plugin;
						}
					}

					if(!devicePlugin)
					{
						outputStream << app.translate("ProxyServerMain",
													"Proxy server unable to find a plugin for device %1\n"
													"Use -devicelist to see available devices.\n").
													arg(deviceName);
					}
					else
					{
						outputStream << app.translate("ProxyServerMain","Starting %1.\n").
														arg(Picto::Names->proxyServerAppName);
						outputStream << app.translate("ProxyServerMain","Device: %1\n").
														arg(deviceName);
						outputStream << app.translate("ProxyServerMain","Type \"quit\" on a single line to exit\n");
						outputStream.flush();

						InteractiveSTDIOHandler interactiveSTDIOHandler;
						interactiveSTDIOHandler.start();

						result = serviceMain(devicePlugin);

						outputStream << app.translate("ProxyServerMain","Shutting down...\n");
					}
				}

				else if(argument.mid(1) == "listdevices")
				{
					//Run through the plugins printing their names 
					//(both static and dynamic plugins)
					outputStream << app.translate("ProxyServerMain", "Available device plugins:\n");
					foreach (QObject *plugin, QPluginLoader::staticInstances())
					{
						NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(plugin);
						outputStream << QString("%1\n").arg(iNDAcq->device());
					}
					
					QDir pluginsDir = QDir(qApp->applicationDirPath());

	#if defined(Q_OS_WIN)
					if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
						pluginsDir.cdUp();
	#elif defined(Q_OS_MAC)
					if (pluginsDir.dirName() == "MacOS") {
						pluginsDir.cdUp();
						pluginsDir.cdUp();
						pluginsDir.cdUp();
					}
	#endif
					pluginsDir.cd("plugins");

					foreach (QString fileName, pluginsDir.entryList(QDir::Files)) 
					{
						QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
						QObject *plugin = loader.instance();
						if (plugin) 
						{
							NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(plugin);
							outputStream << QString("%1\n").arg(iNDAcq->device());
						}
					}




				}
				else
				{
					outputStream << usageDescription;
				}
			}
		}
	}*/

	Picto::CloseLib();

	return 0;
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