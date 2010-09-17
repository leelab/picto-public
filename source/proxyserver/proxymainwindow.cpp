#include <QtGui>

#include "proxymainwindow.h"
#include "NeuralDataAcqInterface.h"
#include "network/proxyserver.h"
#include "protocol/ProxyServerProtocols.h"
#include "protocol/ProxyServerAcqProtocol.h"
#include "../common/globals.h"

ProxyMainWindow::ProxyMainWindow()
{
	acqPlugin_ = NULL;

    setWindowTitle(tr("%1").arg(Picto::Names->proxyServerAppName));

	createStatusLights();
	createButtons();
	createLineEdits();
	createComboBox();
	createLayout();
	createTimer();

	readSettings();

	QWidget *controlPanel = new QWidget(this);
	controlPanel->setLayout(layout_);
	
	setCentralWidget(controlPanel);
}

void ProxyMainWindow::setNeuralDataAcquisitionDevice(int index)
{
	//QAction *action = qobject_cast<QAction *>(sender());
	//acqPlugin_ = action->parent();
	acqPlugin_ = acqPluginList_[index];
	return;
}



void ProxyMainWindow::startStopServer()
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);

	if(startStopServerButton_->text() == startServerMsg)
	{
		//start the Neural Data Acquisition Device
		iNDAcq->startDevice();
		if(iNDAcq->getDeviceStatus() != NeuralDataAcqInterface::running)
		{
			QMessageBox startErrorBox;
			QString errorMsg = tr("%1 failed to start server.  "
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

		
		//set up the servers (HTTP and ACQ)
		QSharedPointer<ProxyServerProtocols> acqProtocols(new ProxyServerProtocols());

		QSharedPointer<ProxyServerAcqProtocol> acqProtocol(new ProxyServerAcqProtocol(acqPlugin_));
		
		acqProtocols->addProtocol(acqProtocol);

		/*! \todo this should specify the IP address in addition to the port, and both should be read from the
		 *        configuration database.
		 */
		port_ = 42420;
		ProxyServer acqServer(port_, acqProtocols,this);

		connect(&acqServer,SIGNAL(activity()),this, SLOT(serverActivity()));

		pluginCombo_->setEnabled(false);
		lineEditName_->setEnabled(false);
		startStopServerButton_->setText(stopServerMsg_);
		readyStatus_->turnGreen();

		announce();

		serverEventLoop_ = new QEventLoop();
		serverEventLoop_->exec();


	}
	else if(startStopServerButton_->text() == stopServerMsg_)
	{
		serverEventLoop_->exit();
		startStopServerButton_->setText(startServerMsg);
		iNDAcq->stopDevice();
		pluginCombo_->setEnabled(true);
		lineEditName_->setEnabled(true);

		//Announce our departure to the world (or at least our network)
		depart();
		readyStatus_->turnRed();


	}
	return;
}

//This is called every 5 seconds just to confirm that the
//neural acquisition device hasn't failed out from under us.
void ProxyMainWindow::checkDevStatus()
{
	//Is the server supposed to be running?
	if(startStopServerButton_->text() == startServerMsg)
		return;

	//Is the device running?
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
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

void ProxyMainWindow::closeEvent(QCloseEvent *ev)
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	if(iNDAcq)
		iNDAcq->stopDevice();

	//Announce our departure to the world (or at least our network)
	depart();

	writeSettings();
	
	//accept the close event
	ev->accept();
}

void ProxyMainWindow::serverActivity()
{
	activityTimer_->start();
	activityStatus_->turnGreen();
}

void ProxyMainWindow::createStatusLights()
{
	readyStatus_ = new StatusLight(this,Qt::red,10);
	activityStatus_ = new StatusLight(this,Qt::red,10);

	readyStatusLabel_ = new QLabel(tr("Ready"));
	activityStatusLabel_ = new QLabel(tr("Activity"));

}

void ProxyMainWindow::createComboBox()
{
	pluginCombo_ = new QComboBox();
	pluginCombo_->setEditable(false);

	//Run through the plugins adding them to the combo box as we go 
	//(both static and dynamic plugins)
	foreach (QObject *plugin, QPluginLoader::staticInstances())
	{
		NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(plugin);
		if(iNDAcq)
		{
			//QAction *action = new QAction(iNDAcq->device(), plugin);
			//connect(action, SIGNAL(triggered()),this, SLOT(setNeuralDataAcquisitionDevice()));
			//pluginCombo_->addAction(action);
			pluginCombo_->addItem(iNDAcq->device());
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
		//QString error = loader.errorString();
		if (plugin) 
		{
			NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(plugin);
			if(iNDAcq)
			{
				pluginCombo_->addItem(iNDAcq->device());
				acqPluginList_<<plugin;
			}
		}

	}

	if(acqPluginList_.length() == 0)
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

		startStopServerButton_->setEnabled(false);

		return;
	}

	setNeuralDataAcquisitionDevice(0);
	connect(pluginCombo_,SIGNAL(currentIndexChanged(int)),this,SLOT(setNeuralDataAcquisitionDevice(int)));

	return;
}

void ProxyMainWindow::createButtons()
{
	startServerMsg = tr("&Start server");
	stopServerMsg_ = tr("&Stop server");
	startStopServerButton_ = new QPushButton(startServerMsg);
	startStopServerButton_->setDefault(true);

	quitButton_ = new QPushButton(tr("&Quit"));
	quitButton_->setDefault(false);

	connect(startStopServerButton_,SIGNAL(clicked()),this,SLOT(startStopServer()));
	connect(quitButton_,SIGNAL(clicked()),this,SLOT(close()));
}

void ProxyMainWindow::createLineEdits()
{
	lineEditName_ = new QLineEdit();
	lineEditName_->setText("proxyName");
	lineEditNameLabel_ = new QLabel(tr("&Proxy Name:"));
	lineEditNameLabel_->setBuddy(lineEditName_);
}

void ProxyMainWindow::createLayout()
{
	QHBoxLayout *HLayout;
	layout_ = new QVBoxLayout();

	HLayout = new QHBoxLayout();
	HLayout->addWidget(lineEditNameLabel_);
	HLayout->addWidget(lineEditName_);
	layout_->addLayout(HLayout);

	HLayout = new QHBoxLayout();
	HLayout->addWidget(readyStatus_);
	HLayout->addWidget(readyStatusLabel_);
	layout_->addLayout(HLayout);
	
	HLayout = new QHBoxLayout();
	HLayout->addWidget(activityStatus_);
	HLayout->addWidget(activityStatusLabel_);
	layout_->addLayout(HLayout);

	layout_->addWidget(pluginCombo_);
	layout_->addWidget(startStopServerButton_);
	layout_->addWidget(quitButton_);
}

void ProxyMainWindow::createTimer()
{
	QTimer *statusTimer = new QTimer(this);
	connect(statusTimer,SIGNAL(timeout()),this,SLOT(checkDevStatus()));
	statusTimer->start(5000);

	activityTimer_ = new QTimer();
	activityTimer_->setInterval(2000);
	activityTimer_->setSingleShot(true);
	connect(activityTimer_,SIGNAL(timeout()),activityStatus_,SLOT(turnRed()));

}


/*****************************************************
 *
 *			PERSISTANT SETTINGS
 *
 *****************************************************/
/*! \Brief stores settings
 *
 *	Settings for the workstation app are stored between sessions.
 *	The QSettings object does this for us in a platform independent
 *	manner.  This uses the registry in Windows, XML preference files
 *	in OSX, and ini files in Unix.
 */
void ProxyMainWindow::writeSettings()
{
	QSettings settings("Block Designs", Picto::Names->proxyServerAppName);

	settings.setValue("proxyName",lineEditName_->text());

}

void ProxyMainWindow::readSettings()
{
	QSettings settings("Block Designs", Picto::Names->proxyServerAppName);

	QString proxyName = settings.value("proxyName").toString();

	if(!proxyName.isNull())
		lineEditName_->setText(proxyName);
}

void ProxyMainWindow::announce()
{
	//Announce our prescence to the world (or at least our network)
	QUdpSocket udpSendSocket;

	QString proxyName = lineEditName_->text().remove(' ');

	QByteArray datagram = QString("ANNOUNCE %1:%2 ACQ/1.0").arg(proxyName).arg(port_).toAscii();

	udpSendSocket.writeDatagram(datagram.data(), datagram.size(),
								QHostAddress::Broadcast, 42424);

}

void ProxyMainWindow::depart()
{
	//Announce our departure to the world (or at least our network)
	QUdpSocket udpSendSocket;

	QString proxyName = lineEditName_->text().remove(' ');

	QByteArray datagram = QString("DEPART %1:%2 ACQ/1.0").arg(proxyName).arg(port_).toAscii();

	udpSendSocket.writeDatagram(datagram.data(), datagram.size(),
								QHostAddress::Broadcast, 42424);

}