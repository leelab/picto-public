#include <QtGui>

#include "proxymainwindow.h"
#include "NeuralDataAcqInterface.h"
#include "network/proxyserver.h"
#include "protocol/ProxyServerProtocols.h"
#include "protocol/ProxyServerAcqProtocol.h"
#include "../common/globals.h"
#include "../common/network/CommandChannel.h"
#include "../common/timing/timestamper.h"
#include "protocol/ProxyNewSessionResponseHandler.h"
#include "network/ProxyStatusManager.h"
#include "../common/protocol/ProtocolResponseHandler.h"
#include "../common/memleakdetect.h"

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

	readSettings();

	QWidget *controlPanel = new QWidget(this);
	controlPanel->setLayout(layout_);
	
	setCentralWidget(controlPanel);






	currState_ = InitState;
	stateTrigger_ = NoProxyTrigger;
	stateUpdateTimer_ = new QTimer(this);
	stateUpdateTimer_->setInterval(0);
	connect(stateUpdateTimer_,SIGNAL(timeout()),this,SLOT(updateState()));
	updatingState_ = false;

	//This timer triggers each new run of updateState()
	stateUpdateTimer_->start();

	//Since the startConnecting function effectively never returns, we can't
	//call it from here, or else this constructor will never return.  We can't
	//start it in the updateState() function, or that function will only get
	//called once and never exit.  We therefore create a single shot timer that
	//calls that function such that it will be called by the top level event
	//delivery framework
	QTimer::singleShot(1,this,SLOT(startConnecting()));
}

//! \brief Sets the plugin to the value in the plugin list
void ProxyMainWindow::setNeuralDataAcquisitionDevice(int index)
{
	if(acqPlugin_ != acqPluginList_[index])
	{
		NeuralDataAcqInterface *iNDAcq;
		if(acqPlugin_)
		{
			iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
			iNDAcq->stopDevice();
		}
		acqPlugin_ = acqPluginList_[index];
		iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
		iNDAcq->deviceSelected();
	}
	if(statusManager_)
		statusManager_.staticCast<ProxyStatusManager>()->setPlugin(acqPlugin_);

	return;
}

bool ProxyMainWindow::deviceIsRunning()
{
	//Is the device running?
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	if(!iNDAcq)
		return false;
	return iNDAcq->getDeviceStatus() > NeuralDataAcqInterface::notStarted;
}

//! \brief This is called to confirm that the neural acquisition is running, or attempt to start it running.
bool ProxyMainWindow::deviceHasData()
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	if(!iNDAcq)
		return false;
	bool returnVal = false;
	switch(iNDAcq->getDeviceStatus())
	{
	case NeuralDataAcqInterface::notStarted:
	case NeuralDataAcqInterface::started:
	case NeuralDataAcqInterface::noData:
		returnVal = false;
		break;
	case NeuralDataAcqInterface::hasData:
		returnVal = true;
	}
	return returnVal;
}

bool ProxyMainWindow::startDevice()
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	if(!iNDAcq)
		return false;
	return (iNDAcq->startDevice() > NeuralDataAcqInterface::notStarted);
}

void ProxyMainWindow::stopDevice()
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	if(!iNDAcq)
		return;
	iNDAcq->stopDevice();
}

//! \brief This is called to confirm that the server connection is active and reconnect if its not.
bool ProxyMainWindow::isServerConnected()
{
	if(dataCommandChannel_.isNull() || !dataCommandChannel_->assureConnection())
	{
		return false;
	}
	return true;
	
}

//! \brief Comfirms that the proxy is in a session.
bool ProxyMainWindow::isSessionActive()
{
	if(statusManager_.isNull())
		return false;
	ComponentStatus status = statusManager_->getStatus();
	if((status == idle) || (status == ending))
	{
		return false;
	}
	return true;
}

void ProxyMainWindow::closeEvent(QCloseEvent *event)
{
	if(currState_ > WaitForSession)
	{	//We're in a session.  Assure that the proxy should be closed.
		
		int	r = QMessageBox::warning(this,Picto::Names->workstationAppName,
			tr("The proxy cannot reconnect to a session once it has been closed."
			"  Are you sure you want to quit?"),
			   QMessageBox::Yes|QMessageBox::No);

		if(r == QMessageBox::No)
		{
			return event->ignore();
		}
	}

	if(!statusManager_.isNull())
	{
		statusManager_->forceExit();
	}
	writeSettings();
	event->accept();
}

void ProxyMainWindow::createStatusLights()
{
	connectionStatus_ = new StatusLight(this,Qt::red,10);
	sessionStatus_ = new StatusLight(this,Qt::red,10);
	runStatus_ = new StatusLight(this,Qt::red,10);

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
		qDebug(pluginsDir.absoluteFilePath(fileName).toAscii());
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

		//startStopClientButton_->setEnabled(false);

		return;
	}

	setNeuralDataAcquisitionDevice(0);
	//connect(pluginCombo_,SIGNAL(currentIndexChanged(int)),this,SLOT(setNeuralDataAcquisitionDevice(int)));

	return;
}

void ProxyMainWindow::createButtons()
{
	//connect(startStopClientButton_,SIGNAL(clicked()),this,SLOT(startStopClient()));
	//connect(quitButton_,SIGNAL(clicked()),this,SLOT(close()));
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
	//layout_->addWidget(startStopClientButton_);
	//layout_->addWidget(quitButton_);
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
	statusManager_ = QSharedPointer<ComponentStatusManager>(new ProxyStatusManager(dataCommandChannel_));
	dataCommandChannel_->addResponseHandler(QSharedPointer<Picto::ProtocolResponseHandler>(new ProxyNewSessionResponseHandler(statusManager_,dataCommandChannel_)));
	return 0;
}
int ProxyMainWindow::closeDevice()
{
	return 0;
}










void ProxyMainWindow::updateState()
{
	updatingState_ = true;
	ProxyState nextState = currState_;
	switch(currState_)
	{
	case InitState:
		nextState = WaitForConnect;
		break;
	case WaitForConnect:
		switch(stateTrigger_)
		{
		case Connected:
			nextState = WaitForSession;
			break;
		}
		break;
	case WaitForSession:
		switch(stateTrigger_)
		{
		case Disconnected:
			nextState = WaitForConnect;
			break;
		case ConnectDeviceRequest:
			nextState = WaitForDevice;
			break;
		}
		break;
	case WaitForDevice:
		switch(stateTrigger_)
		{
		case SessionEnded:
			nextState = WaitForSession;
			break;
		case DeviceStarted:
			nextState = Running;
			break;
		}
		break;
	case Running:
		switch(stateTrigger_)
		{
		case DeviceStopped:
			nextState = WaitForDevice;
			break;
		case SessionEnded:
			nextState = WaitForSession;
		}
		break;
	}
	stateTrigger_ = NoProxyTrigger;
	if(nextState != currState_)
	{
		currState_ = nextState;
		enterState();
	}
	runState();
	if(stateTrigger_ != NoProxyTrigger)
		endState();
	//updateStatus();
	updatingState_ = false;
}
void ProxyMainWindow::startConnecting()
{
	activate();
}

void ProxyMainWindow::runState()
{
	switch(currState_)
	{
	case WaitForConnect:
		if(statusManager_)
			statusManager_.staticCast<ProxyStatusManager>()->setName(name());
		if(!dataCommandChannel_.isNull() && dataCommandChannel_->assureConnection())
			stateTrigger_ = Connected;
		break;
	case WaitForSession:
		setNeuralDataAcquisitionDevice(pluginCombo_->currentIndex());
		if(statusManager_)
			statusManager_.staticCast<ProxyStatusManager>()->setName(name());
		if(dataCommandChannel_.isNull() || !dataCommandChannel_->assureConnection())
			stateTrigger_ = Disconnected;
		else if(isSessionActive())
			stateTrigger_ = ConnectDeviceRequest;
		break;
	case WaitForDevice:
		if(deviceIsRunning())
		{
			if(deviceHasData())
				stateTrigger_ = DeviceStarted;
		}
		else
		{
			startDevice();
		}
		if(!isSessionActive())
		{
			stateTrigger_ = SessionEnded;
			stopDevice();
		}
		break;
	case Running:
		if(!deviceHasData())
			stateTrigger_ = DeviceStopped;
		if(!isSessionActive())
		{
			stateTrigger_ = SessionEnded;
			stopDevice();
		}
		break;
	}
	if(currState_ != WaitForConnect)
	{
		if(isServerConnected())
			connectionStatus_->turnGreen();
		else
			connectionStatus_->turnRed();
	}
}

void ProxyMainWindow::endState()
{
	switch(currState_)
	{
	case WaitForConnect:
		break;
	case WaitForSession:
		break;
	case WaitForDevice:
		break;
	case Running:
		break;
	}
}

void ProxyMainWindow::enterState()
{
	switch(currState_)
	{
	case WaitForConnect:
		lineEditName_->setEnabled(true);
		runStatus_->turnRed();
		sessionStatus_->turnRed();
		pluginCombo_->setEnabled(true);
		qDebug("Entering WaitForConnect");
		break;
	case WaitForSession:
		lineEditName_->setEnabled(true);
		runStatus_->turnRed();
		sessionStatus_->turnRed();
		pluginCombo_->setEnabled(true);
		qDebug("Entering WaitForSession");
		break;
	case WaitForDevice:
		lineEditName_->setEnabled(false);
		runStatus_->turnRed();
		sessionStatus_->turnGreen();
		pluginCombo_->setEnabled(true);
		qDebug("Entering WaitForDevice");
		break;
	case Running:
		lineEditName_->setEnabled(false);
		runStatus_->turnGreen();
		sessionStatus_->turnGreen();
		pluginCombo_->setEnabled(false);
		qDebug("Entering Running");
		break;
	}
}
