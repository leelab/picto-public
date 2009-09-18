/*! \file mainwindow.cpp
 * \ingroup pictoproxyserver
 * \brief A GUI window for controlling the proxy server
 */

#include <QtGui>

#include "mainwindow.h"
#include "interfaces.h"
#include "network/server.h"
#include "protocol/ServerProtocols.h"
#include "protocol/ServerAcqProtocol.h"
#include "protocol/ServerHTTPProtocol.h"
#include "../common/globals.h"

MainWindow::MainWindow()
{
	acqPlugin = NULL;

    setWindowTitle(tr("%1").arg(Picto::Names->proxyServerAppName));

	createComboBox();
	createButtons();
	createLayout();
	createTimer();

	QWidget *controlPanel = new QWidget(this);
	controlPanel->setLayout(layout);

	setCentralWidget(controlPanel);
}

void MainWindow::setNeuralDataAcquisitionDevice(int index)
{
	//QAction *action = qobject_cast<QAction *>(sender());
	//acqPlugin = action->parent();
	acqPlugin = acqPluginList[index];
	return;
}



void MainWindow::startStopServer()
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin);

	if(startStopServerButton->text() == startServerMsg)
	{
		//start the Neural Data Acquisition Device
		iNDAcq->startDevice();
		if(iNDAcq->getDeviceStatus() != NeuralDataAcqInterface::running)
		{
			QMessageBox startErrorBox;
			QString errorMsg = tr("%1 failed to start server."
								  "Device: \"%2\" is not running.  Confirm that "
								  "all the needed hardware and software is turned "
								  "on and started up, then try again.").
								  arg(Picto::Names->proxyServerAppName).
								  arg(iNDAcq->device());
			startErrorBox.setText(tr("Server failed to start"));
			startErrorBox.setInformativeText(errorMsg);
			startErrorBox.exec();
			return;
		}

		QSharedPointer<ServerProtocols> httpProtocols(new ServerProtocols());
		QSharedPointer<ServerProtocols> acqProtocols(new ServerProtocols());

		QSharedPointer<ServerHTTPProtocol> httpProtocol(new ServerHTTPProtocol(acqPlugin));
		QSharedPointer<ServerAcqProtocol> acqProtocol(new ServerAcqProtocol(acqPlugin));
		
		httpProtocols->addProtocol(httpProtocol);
		acqProtocols->addProtocol(httpProtocol);
		acqProtocols->addProtocol(acqProtocol);


		/*! \todo this should specify the IP address in addition to the port, and both should be read from the
		 *        configuration database.
		 */
		Server httpServer(80, httpProtocols);
		Server acqServer(42424, acqProtocols);

		pluginCombo->setEnabled(false);
		startStopServerButton->setText(stopServerMsg);


		serverEventLoop = new QEventLoop();
		serverEventLoop->exec();
	}
	else if(startStopServerButton->text() == stopServerMsg)
	{
		serverEventLoop->exit();
		startStopServerButton->setText(startServerMsg);
		iNDAcq->stopDevice();
		pluginCombo->setEnabled(true);
	}
	return;
}

//This is called every 5 seconds just to confirm that the
//neural acquisition device hasn't failed out from under us.
void MainWindow::checkDevStatus()
{
	//Is the server running?
	if(startStopServerButton->text() == startServerMsg)
		return;

	//Is the device running?
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin);
	if(iNDAcq->getDeviceStatus() == NeuralDataAcqInterface::running)
		return;
	
	//attempt to recover
	iNDAcq->stopDevice();
	iNDAcq->startDevice();

	//did we recover?
	if(iNDAcq->getDeviceStatus() == NeuralDataAcqInterface::running)
		return;

	//stop device and server
	iNDAcq->stopDevice();
	startStopServer();

	//put up a message box
	QMessageBox deviceErrorBox;
	QString errorMsg = tr("Device \"%2\" has stopped running.  Confirm that "
						  "all the needed hardware and software is turned "
						  "on and started up, then restart the server.").
						  arg(iNDAcq->device());
	deviceErrorBox.setText(tr("Device not running"));
	deviceErrorBox.setInformativeText(errorMsg);
	deviceErrorBox.exec();
	return;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin);
	iNDAcq->stopDevice();
	serverEventLoop->exit();
	event->accept();
}
void MainWindow::createButtons()
{
	startServerMsg = tr("Start server");
	stopServerMsg = tr("Stop server");
	startStopServerButton = new QPushButton(startServerMsg);

	quitButton = new QPushButton(tr("Quit"));

	connect(startStopServerButton,SIGNAL(clicked()),this,SLOT(startStopServer()));
	connect(quitButton,SIGNAL(clicked()),this,SLOT(close()));
}
void MainWindow::createLayout()
{
	layout = new QVBoxLayout();
	layout->addWidget(pluginCombo);
	layout->addWidget(startStopServerButton);
	layout->addWidget(quitButton);
}

void MainWindow::createTimer()
{
	QTimer *statusTimer = new QTimer(this);
	connect(statusTimer,SIGNAL(timeout()),this,SLOT(checkDevStatus()));
	statusTimer->start(5000);
}



void MainWindow::createComboBox()
{
	pluginCombo = new QComboBox();
	pluginCombo->setEditable(false);

	//Run through the plugins adding them to the combo box as we go 
	//(both static and dynamic plugins)
	foreach (QObject *plugin, QPluginLoader::staticInstances())
	{
		NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(plugin);
		if(iNDAcq)
		{
			//QAction *action = new QAction(iNDAcq->device(), plugin);
			//connect(action, SIGNAL(triggered()),this, SLOT(setNeuralDataAcquisitionDevice()));
			//pluginCombo->addAction(action);
			pluginCombo->addItem(iNDAcq->device());
		}
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
		QString error = loader.errorString();
		if (plugin) 
		{
			NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(plugin);
			if(iNDAcq)
			{
				//QAction *action = new QAction(iNDAcq->device(), plugin);
				//connect(action, SIGNAL(triggered()),this, SLOT(setNeuralDataAcquisitionDevice()));
				//pluginCombo->addAction(action);
				pluginCombo->addItem(iNDAcq->device());
				acqPluginList<<plugin;
			}
		}
	}

	if(acqPluginList.length() == 0)
	{
		QMessageBox noPluginMsgBox;
		noPluginMsgBox.setText(tr("Plugins not found"));
		QString errorMsg = tr("No plugins for neural acquisition devices were found. "
							"Make sure there are %1 plugins in %2, then quit and restart "
							"%1").
							  arg(Picto::Names->proxyServerAppName).
							  arg(pluginsDir.absolutePath());
		noPluginMsgBox.setInformativeText(errorMsg);
		noPluginMsgBox.exec();

		return;
	}

	setNeuralDataAcquisitionDevice(0);
	connect(pluginCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(setNeuralDataAcquisitionDevice(int)));

	return;
}
