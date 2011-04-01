#include <QtGui>

#include "proxymainwindow.h"
#include "NeuralDataAcqInterface.h"
#include "network/proxyserver.h"
#include "protocol/ProxyServerProtocols.h"
#include "protocol/ProxyServerAcqProtocol.h"
#include "../common/globals.h"
#include "../common/network/CommandChannel.h"
#include "../common/timing/timestamper.h"
#include "protocol/ProxyStartResponseHandler.h"
#include "network/ProxyStatusManager.h"
#include "../common/protocol/ProtocolResponseHandler.h"

ProxyMainWindow::ProxyMainWindow()
:ComponentInterface("PROXY")
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

//! \brief Sets the plugin to the value in the plugin list
void ProxyMainWindow::setNeuralDataAcquisitionDevice(int index)
{
	acqPlugin_ = acqPluginList_[index];
	return;
}

//! \brief Called by the button signal, this starts or stops ProxyClient activity.
void ProxyMainWindow::startStopClient()
{
	if(startStopClientButton_->text() == startServerMsg)
		activate();
	else
		deActivate();
}

//! \brief This is called every 5 seconds just to confirm that the neural acquisition device hasn't failed out from under us.
void ProxyMainWindow::checkDevStatus()
{
	//Is the server supposed to be running?
	if(startStopClientButton_->text() == startServerMsg)
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
	startStopClient();

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
//! \brief This is called every 5 seconds just to confirm that the server connection is still active.
void ProxyMainWindow::checkConnectionStatus()
{
	//Is the server supposed to be running?
	if(startStopClientButton_->text() == startServerMsg)
		return;

	if(dataCommandChannel_.isNull() || !dataCommandChannel_->assureConnection())
	{
		connectionStatus_->turnRed();
		return;
	}
	connectionStatus_->turnGreen();
	
}

//! \brief This is called every 5 seconds just to confirm that the proxy is in a session.
void ProxyMainWindow::checkSessionStatus()
{
	//Is the server supposed to be running?
	if(startStopClientButton_->text() == startServerMsg)
		return;

	if(statusManager_.isNull() || (statusManager_->getStatus() < stopped))
	{
		sessionStatus_->turnRed();
		return;
	}
	sessionStatus_->turnGreen();
}

//! \brief This is called every 5 seconds just to confirm that the proxy is running.
void ProxyMainWindow::checkRunStatus()
{
	//Is the server supposed to be running?
	if(startStopClientButton_->text() == startServerMsg)
		return;

	if(statusManager_.isNull() || (statusManager_->getStatus() <= stopped))
	{
		runStatus_->turnRed();
		return;
	}
	runStatus_->turnGreen();
	
}

//! \brief Called when the window is closed, this ends all activity and saves settings
void ProxyMainWindow::closeEvent(QCloseEvent *ev)
{
	deActivate();

	writeSettings();
	//accept the close event
	ev->accept();
}

//! \brief This signals to the UI that activity is detected from the server.
void ProxyMainWindow::serverActivity()
{
	activityTimer_->start();
	connectionStatus_->turnGreen();
}

void ProxyMainWindow::createStatusLights()
{
	readyStatus_ = new StatusLight(this,Qt::red,10);
	connectionStatus_ = new StatusLight(this,Qt::red,10);
	sessionStatus_ = new StatusLight(this,Qt::red,10);
	runStatus_ = new StatusLight(this,Qt::red,10);

	readyStatusLabel_ = new QLabel(tr("Ready"));
	connectionStatusLabel_ = new QLabel(tr("Connected"));
	sessionStatusLabel_ = new QLabel(tr("In Session"));
	runStatusLabel_ = new QLabel(tr("Running"));

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

		startStopClientButton_->setEnabled(false);

		return;
	}

	setNeuralDataAcquisitionDevice(0);
	connect(pluginCombo_,SIGNAL(currentIndexChanged(int)),this,SLOT(setNeuralDataAcquisitionDevice(int)));

	return;
}

void ProxyMainWindow::createButtons()
{
	startServerMsg = tr("&Start");
	stopServerMsg_ = tr("&Stop");
	startStopClientButton_ = new QPushButton(startServerMsg);
	startStopClientButton_->setDefault(true);

	quitButton_ = new QPushButton(tr("&Quit"));
	quitButton_->setDefault(false);

	connect(startStopClientButton_,SIGNAL(clicked()),this,SLOT(startStopClient()));
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
	HLayout->addWidget(connectionStatus_);
	HLayout->addWidget(connectionStatusLabel_);
	layout_->addLayout(HLayout);

	HLayout = new QHBoxLayout();
	HLayout->addWidget(sessionStatus_);
	HLayout->addWidget(sessionStatusLabel_);
	layout_->addLayout(HLayout);

	HLayout = new QHBoxLayout();
	HLayout->addWidget(runStatus_);
	HLayout->addWidget(runStatusLabel_);
	layout_->addLayout(HLayout);

	layout_->addWidget(pluginCombo_);
	layout_->addWidget(startStopClientButton_);
	layout_->addWidget(quitButton_);
}

void ProxyMainWindow::createTimer()
{
	QTimer *statusTimer = new QTimer(this);
	connect(statusTimer,SIGNAL(timeout()),this,SLOT(checkDevStatus()));
	connect(statusTimer,SIGNAL(timeout()),this,SLOT(checkConnectionStatus()));
	connect(statusTimer,SIGNAL(timeout()),this,SLOT(checkSessionStatus()));
	connect(statusTimer,SIGNAL(timeout()),this,SLOT(checkRunStatus()));
	statusTimer->start(5000);

	activityTimer_ = new QTimer();
	activityTimer_->setInterval(2000);
	activityTimer_->setSingleShot(true);
	connect(activityTimer_,SIGNAL(timeout()),connectionStatus_,SLOT(turnRed()));

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


QString ProxyMainWindow::name()
{
	QString name = lineEditName_->text().remove(' ');
	if(name == QString("proxyName"))
		name = QString("Proxy").append(componentId_.toString());
	return name;
}
int ProxyMainWindow::openDevice()
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
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
		return 1;
	}
	pluginCombo_->setEnabled(false);
	lineEditName_->setEnabled(false);
	startStopClientButton_->setText(stopServerMsg_);
	readyStatus_->turnGreen();
	statusManager_ = QSharedPointer<ComponentStatusManager>(new ProxyStatusManager(dataCommandChannel_));
	dataCommandChannel_->addResponseHandler(QSharedPointer<Picto::ProtocolResponseHandler>(new ProxyStartResponseHandler(statusManager_,dataCommandChannel_,acqPlugin_)));
	return 0;
}
int ProxyMainWindow::closeDevice()
{
	startStopClientButton_->setText(startServerMsg);

	pluginCombo_->setEnabled(true);
	lineEditName_->setEnabled(true);
	readyStatus_->turnRed();
	return 0;
}