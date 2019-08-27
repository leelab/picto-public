#include <QtWidgets>
#include <QProcessEnvironment>

#include "proxymainwindow.h"
#include "NeuralDataAcqInterface.h"
#include "../common/globals.h"
#include "../common/network/CommandChannel.h"
#include "../common/timing/timestamper.h"
#include "protocol/ProxyNewSessionResponseHandler.h"
#include "network/ProxyStatusManager.h"
#include "../common/protocol/ProtocolResponseHandler.h"
#include "../common/memleakdetect.h"

/*! \brief Constrcuts a ProxyMainWindow object.
 *	\details Sets up the ProxyMainWindow UI, sets up a timer to periodically call updateState() to update
 *	this object's state machine, and schedules a call to startConnecting() from the Qt Event Loop
 *	so that the ComponentInterface's activate() function will be called and the application will run.
 */
ProxyMainWindow::ProxyMainWindow()
:ComponentInterface("PROXY")
{
	acqPlugin_ = NULL;

    setWindowTitle(tr("%1").arg(Picto::Names->proxyServerAppName));

	createStatusLights();
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

/*! \brief Sets the active plugin to the one at the input index in the plugin list.
 *	\details If another plugin is currently active, it is stopped first.
 */
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
		qDebug() << "Starting device index: " << index;
	}
	if(statusManager_)
		statusManager_.staticCast<ProxyStatusManager>()->setPlugin(acqPlugin_);

	return;
}

/*! \brief Returns true if the Neural Data plugin has been started by this system (ie. Data from the 
*	current time will be gathered for the session).
*/
bool ProxyMainWindow::deviceIsRunning()
{
	//Is the device running?
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	if(!iNDAcq)
		return false;
	return iNDAcq->getDeviceStatus() > NeuralDataAcqInterface::notStarted;
}

/*! \brief Returns true if the neural data plugin is running and there is data available for gathering
 *	from the Neural Data Acquisition device with which it is interfaceing.
 */
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

/*! \brief Tells the the Neural Data plugin to start gathering data.
 *	\details Returns false if the plugin was not able to start, true otherwise.
 */
bool ProxyMainWindow::startDevice()
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	if(!iNDAcq)
		return false;
	return (iNDAcq->startDevice() > NeuralDataAcqInterface::notStarted);
}

/*! \brief Tells the Neural Data plugin to stop gathering data.
 */
void ProxyMainWindow::stopDevice()
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	if(!iNDAcq)
		return;
	iNDAcq->stopDevice();
}

/*! \brief Returns true if the Picto Server is connected.  If it isn't this function uses
 *	CommandChannel::assureConnection() to restart the connection process.
 */
bool ProxyMainWindow::isServerConnected()
{
	if(dataCommandChannel_.isNull() || !dataCommandChannel_->assureConnection())
	{
		return false;
	}
	return true;
	
}

/*! \brief Returns true if this Proxy is in a Session.
 */
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

/*! \brief Called when the Proxy window is closed to cause the main data load/send loop in 
 *	ProxyNewSessionResponseHandler::processResponse() to exit.
 *	\details Before causing that loop to exit, if we are in a session a pop-up makes sure that
 *	the user understands that the proxy cannot reconnect to the session once it has been closed.
 */
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
}

/*! \brief Creates the status light widgets/labels that are used in the UI.
*/
void ProxyMainWindow::createStatusLights()
{
	connectionStatus_ = new StatusLight(this,Qt::red,10);
	sessionStatus_ = new StatusLight(this,Qt::red,10);
	runStatus_ = new StatusLight(this,Qt::red,10);

	connectionStatusLabel_ = new QLabel(tr("Connected"));
	sessionStatusLabel_ = new QLabel(tr("In Session"));
	runStatusLabel_ = new QLabel(tr("Running"));

}

/*! \brief Creates the UI combobox used to select the current Neural Data plugin.
 *	\details Plugins are loaded from the "\plugins" sub-directory of the application
 *	directory.
 */
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
	
	QDir pluginsDir = QDir(qApp->applicationDirPath() + "/plugins");
	//Set the application path to the plugins folder so that any 3rd party dlls (ie. plexclient.dll)
	//will get loaded from that path.
	QString appPath = QDir::current().absolutePath();
	QDir::setCurrent(pluginsDir.absolutePath());

	QStringList nameFilters;
	nameFilters << "*.dll"; //WARNING, this is platform dependant!!
	foreach (QString fileName, pluginsDir.entryList(nameFilters,QDir::Files)) 
	{
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();
		if (!plugin)
		{
			QString error = loader.errorString();
			qDebug(QString("Problem loading %1: %2").arg(pluginsDir.absoluteFilePath(fileName)).arg(error).toLatin1());
		}
		if (plugin) 
		{
			NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(plugin);
			if(iNDAcq)
			{
				pluginCombo_->addItem(iNDAcq->device());
				acqPluginList_<<plugin;
			}
			else
			{
				qDebug(QString("Could not cast %1 to NeuralDataAcqInterface").arg(pluginsDir.absoluteFilePath(fileName)).toLatin1());
			}
		}

	}
	//Set application path back to original directory.
	QDir::setCurrent(appPath);

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
	connect(pluginCombo_,SIGNAL(currentIndexChanged(int)),this,SLOT(pluginIndexChanged(int)));

	return;
}

/*! \brief Creates the widgets for entering the Proxy name and displaying/changing the 
 *	Proxy's System Number (Proxy can only communicate with the Picto Server that is 
 *	on the same System number.
 */
void ProxyMainWindow::createLineEdits()
{
	lineEditName_ = new QLineEdit();
	lineEditName_->setText("proxyName");
	lineEditNameLabel_ = new QLabel(tr("&Proxy Name:"));
	lineEditNameLabel_->setBuddy(lineEditName_);
	connect(lineEditName_,SIGNAL(editingFinished()),this,SLOT(writeSettings()));

	systemNumber_ = new QSpinBox();
	systemNumber_->setMaximum(130);
	systemNumber_->setValue(Picto::portNums->getSystemNumber());
	systemNumLabel_ = new QLabel(tr("System Number:"));
	systemNumLabel_->setBuddy(systemNumber_);
	connect(systemNumber_,SIGNAL(valueChanged(int)),this,SLOT(systemNumberChanged(int)));
}

/*! \brief Sets up the UI layout for this widget.*/
void ProxyMainWindow::createLayout()
{
	QHBoxLayout *HLayout;
	layout_ = new QVBoxLayout();

	HLayout = new QHBoxLayout();
	HLayout->addWidget(lineEditNameLabel_);
	HLayout->addWidget(lineEditName_);
	layout_->addLayout(HLayout);

	HLayout = new QHBoxLayout();
	HLayout->addWidget(systemNumLabel_);
	HLayout->addWidget(systemNumber_);
	HLayout->addStretch(1);
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

	pluginUI(true);
}

/*****************************************************
 *
 *			PERSISTANT SETTINGS
 *
 *****************************************************/
/*! \brief Reads the current Proxy settings
 *
 *	Settings for the workstation app are stored between sessions.
 *	The QSettings object does this for us in a platform independent
 *	manner.  This uses the registry in Windows, XML preference files
 *	in OSX, and ini files in Unix.
 */
void ProxyMainWindow::readSettings()
{
	QSettings settings("Block Designs", Picto::Names->proxyServerAppName);

	QString proxyName = settings.value("proxyName").toString();

	if(!proxyName.isNull())
		lineEditName_->setText(proxyName);

	QString plugin = settings.value("plugin").toString();
	
	if(plugin.isEmpty())
		pluginCombo_->setCurrentIndex(0);
	else
		pluginCombo_->setCurrentIndex(pluginCombo_->findText(plugin));

	pluginUI(false);
}

/*! \brief Returns the current Proxy name.  If no name has been set, an automatic one is
 *	generated.
 */
QString ProxyMainWindow::name()
{
	QString name = lineEditName_->text().remove(' ');
	if(name == QString("proxyName"))
		name = QString("Proxy").append(componentId_.toString());
	return name;
}

/*! \brief Implements ComponentInterface::openDevice() to create a ProxyStatusManager, load the Data CommandChannel
 *	into it and add a response handler for NEWSESSION commands.
 *	\details the ProxyNewSessionResponseHandler contains the "main method" that gathers neural data and sends it to
 *	the Picto Server, see ProxyNewSessionResponseHandler::processResponse() for more details.
 */
int ProxyMainWindow::openDevice()
{
	statusManager_ = QSharedPointer<ComponentStatusManager>(new ProxyStatusManager(dataCommandChannel_));
	dataCommandChannel_->addResponseHandler(QSharedPointer<Picto::ProtocolResponseHandler>(new ProxyNewSessionResponseHandler(statusManager_,dataCommandChannel_)));
	return 0;
}

/*! \brief Implements ComponentInterface::closeDevice() to do nothing.*/
int ProxyMainWindow::closeDevice()
{
	return 0;
}

/*! \brief Updates this objects current state based on the latest stateTrigger_ value.
 *	\details stateTrigger_ is set in the runState() function.
*/
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
/*! \brief Calls ComponentInterface::activate() to start listening on the Server interface 
 *	and run the application.  When that returns, exits the application.
 */
void ProxyMainWindow::startConnecting()
{
	activate();
	QCoreApplication::exit();
}

/*! \brief Runs the current state.
 *	\details Depending on this object's logical state and the state of the rest of the application,
 *	different things happen and different transitions occur.
 *	
 *	When we're in the WaitForConnect state, we keep trying to connect to the Picto Server.  When we
 *	succeed, we are transferred to the WaitForSession state.  In that state we set up the Neural 
 *	data plugin, then go back to WaitForConnect if the connection is lost, or go to WaitForDevice if a new
 *	session comes in.  From WaitForDevice we start the device and transition to Running when it has data
 *	available.  Alternatively, we transition back to WaitForSession if the session ends.  From Running
 *	we can transition back to WaitForDevice if device data stops coming in or back to WaitForSession if the 
 *	Session ends.
 *
 *	This function also handles updating of the connection status light.
 */
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

/*! \brief This doesn't actually do anything, but the idea is that it can be used to handle any
 *	logic that needs to be triggered when a state ends.
 */
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

/*! \brief Takes care of various operations when this object enters a new logical state.
 *	\details For example, when we enter the WaitForDevice state, the runStatus light is set red,
 *	and the sessionStatus light is set green.  When we enter the Running state, both of these are
 *	set to green.
 */
void ProxyMainWindow::enterState()
{
	switch(currState_)
	{
	case WaitForConnect:
		lineEditName_->setEnabled(true);
		systemNumber_->setEnabled(true);
		runStatus_->turnRed();
		sessionStatus_->turnRed();
		pluginCombo_->setEnabled(true);
		qDebug("Entering WaitForConnect");
		break;
	case WaitForSession:
		lineEditName_->setEnabled(true);
		systemNumber_->setEnabled(true);
		runStatus_->turnRed();
		sessionStatus_->turnRed();
		pluginCombo_->setEnabled(true);
		qDebug("Entering WaitForSession");
		break;
	case WaitForDevice:
		lineEditName_->setEnabled(false);
		systemNumber_->setEnabled(false);
		runStatus_->turnRed();
		sessionStatus_->turnGreen();
		pluginCombo_->setEnabled(true);
		qDebug("Entering WaitForDevice");
		break;
	case Running:
		lineEditName_->setEnabled(false);
		systemNumber_->setEnabled(false);
		runStatus_->turnGreen();
		sessionStatus_->turnGreen();
		pluginCombo_->setEnabled(false);
		qDebug("Entering Running");
		break;
	}
}

/*! \brief Called when the active plugin changes.  Writes the plugin value setting to disk
 *	so that the same one will be used next time.
 */
void ProxyMainWindow::pluginIndexChanged(int)
{
	pluginUI(false);
	writeSettings();
}

/*! \brief When the user changes the system number, this function implements the change by using
 *	the Picto::PortNums::setSystemNumber() function.
 *	\note This causes the Proxy application to restart.
 */
void ProxyMainWindow::systemNumberChanged(int index)
{
	Picto::portNums->setSystemNumber(QCoreApplication::applicationFilePath(),QCoreApplication::arguments(),index,true);
}

/*! \brief Stores applicaiont settings to disk
 *
 *	Settings for the workstation app are stored between sessions.
 *	The QSettings object does this for us in a platform independent
 *	manner.  This uses the registry in Windows, XML preference files
 *	in OSX, and ini files in Unix.
 *
 *	Stored settings are:
 *	- proxyName: The name of this proxy application
 *	- plugin: The name of the Neural data plugin that is currently being used by the proxy application.
 */
void ProxyMainWindow::writeSettings()
{
	QSettings settings("Block Designs", Picto::Names->proxyServerAppName);

	settings.setValue("proxyName",lineEditName_->text());
	settings.setValue("plugin",pluginCombo_->itemText(pluginCombo_->currentIndex()));
}
void ProxyMainWindow::alignSysTimes()
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	if (!iNDAcq)
		return;

	if (progressBar_)
	{
		progressBar_->setVisible(true);		
		progressBar_->setValue(0);
	}

	QString rhdFileName = directoryComboBoxrhd_->currentText();
	if (convert_)
	{
		QString saveFolderName = directoryComboBoxconvert_->currentText();
		iNDAcq->alignSysTimes(rhdFileName, saveFolderName, progressBar_, "", "", true);

		if (progressBar_) progressBar_->setVisible(false);
		QMessageBox::information(
			this,
			tr("Intan Plugin"),
			tr("Conversion Completed"));

	}
	else
	{
		QString sqlFileName = directoryComboBoxsql_->currentText();
		QString mdaFileName = directoryComboBoxmda_->currentText();

		iNDAcq->alignSysTimes(rhdFileName, "", progressBar_, sqlFileName, mdaFileName, false);

		if (progressBar_) progressBar_->setVisible(false);
		QMessageBox::information(
			this,
			tr("Intan Plugin"),
			tr("Alignment Completed"));

	}
}
void ProxyMainWindow::pluginUI(bool first)
{
	bool toHide = true;

	if (first)
	{
		// 2 options for Intan: align or convert
		convertToMda_ = new QCheckBox("Convert Rhd to Mda");
		connect(convertToMda_, SIGNAL(clicked()), SLOT(convertToMDA()));
		layout_->addWidget(convertToMda_);

		alignNeuralData_ = new QCheckBox("Align Neural Data");
		connect(alignNeuralData_, SIGNAL(clicked()), SLOT(alignNeuralData()));
		layout_->addWidget(alignNeuralData_);

		//Picto sql file for behavioral timestamps
		sqlfileLabel_ = new QLabel("Behavioral Data");
		sqlfileButton_ = new QPushButton("Browse");
		connect(sqlfileButton_, SIGNAL(clicked()), SLOT(browsePictoFiles()));
		directoryComboBoxsql_ = createIntanComboBox(QDir::currentPath());

		QHBoxLayout *layoutSqlFile_ = new QHBoxLayout();
		layoutSqlFile_->addWidget(sqlfileLabel_);
		layoutSqlFile_->addWidget(sqlfileButton_);
		layoutSqlFile_->addWidget(directoryComboBoxsql_);

		layout_->addLayout(layoutSqlFile_);

		//Intan rhd file for neural data
		rhdfileLabel_ = new QLabel("Neural Data");
		rhdfileButton_ = new QPushButton("Browse");
		connect(rhdfileButton_, SIGNAL(clicked()), SLOT(browserhdFiles()));
		directoryComboBoxrhd_ = createIntanComboBox(QDir::currentPath());
		QHBoxLayout* layoutrhdFile = new QHBoxLayout();
		layoutrhdFile->addWidget(rhdfileLabel_);
		layoutrhdFile->addWidget(rhdfileButton_);
		layoutrhdFile->addWidget(directoryComboBoxrhd_);

		layout_->addLayout(layoutrhdFile);

		//folder for conversion to mda output
		convertOutputLabel_ = new QLabel("Saved Output");
		convertOutputButton_ = new QPushButton("Browse");
		connect(convertOutputButton_, SIGNAL(clicked()), SLOT(browseSaveFolder()));
		directoryComboBoxconvert_ = createIntanComboBox(QDir::currentPath());
		QHBoxLayout* layoutconvertFolder = new QHBoxLayout();
		layoutconvertFolder->addWidget(convertOutputLabel_);
		layoutconvertFolder->addWidget(convertOutputButton_);
		layoutconvertFolder->addWidget(directoryComboBoxconvert_);

		layout_->addLayout(layoutconvertFolder);

		//Mountain Sort mda file for neural data
		mdafileLabel_ = new QLabel("Sorted Data");
		mdafileButton_ = new QPushButton("Browse");
		connect(mdafileButton_, SIGNAL(clicked()), SLOT(browseMdaFiles()));
		directoryComboBoxmda_ = createIntanComboBox(QDir::currentPath());
		QHBoxLayout* layoutmdaFile = new QHBoxLayout();
		layoutmdaFile->addWidget(mdafileLabel_);
		layoutmdaFile->addWidget(mdafileButton_);
		layoutmdaFile->addWidget(directoryComboBoxmda_);

		layout_->addLayout(layoutmdaFile);

		alignButton_ = new QPushButton("Proceed");
		connect(alignButton_, SIGNAL(clicked()), SLOT(alignSysTimes()));

		layout_->addWidget(alignButton_);
				
		progressBar_ = new QProgressBar();
		progressBar_->setMaximum(100);
		progressBar_->setMinimum(0);
		progressBar_->setValue(0);
		progressBar_->setVisible(false);
		QHBoxLayout *progressLayout = new QHBoxLayout();		
		layout_->addWidget(progressBar_);
	}

	if (pluginCombo_->currentText() == "Intan")
	{
		toHide = false;
		if ((convertToMda_ && !convertToMda_->isChecked()) || (alignNeuralData_ && !alignNeuralData_->isChecked()))
		{
			if (sqlfileLabel_) sqlfileLabel_->hide();
			if (sqlfileButton_) sqlfileButton_->hide();
			if (directoryComboBoxsql_) directoryComboBoxsql_->hide();
			if (mdafileLabel_) mdafileLabel_->hide();
			if (mdafileButton_) mdafileButton_->hide();
			if (directoryComboBoxmda_) directoryComboBoxmda_->hide();
			if (rhdfileLabel_) rhdfileLabel_->hide();
			if (rhdfileButton_) rhdfileButton_->hide();
			if (directoryComboBoxrhd_) directoryComboBoxrhd_->hide();
			if (convertOutputLabel_) convertOutputLabel_->hide();
			if (convertOutputButton_) convertOutputButton_->hide();
			if (directoryComboBoxconvert_) directoryComboBoxconvert_->hide();
			if (alignButton_) alignButton_->hide();
		}
	}


	if (toHide)
	{
		//hide all for now
		if (sqlfileLabel_) sqlfileLabel_->hide();
		if (sqlfileButton_) sqlfileButton_->hide();
		if (directoryComboBoxsql_) directoryComboBoxsql_->hide();
		if (mdafileLabel_) mdafileLabel_->hide();
		if (mdafileButton_) mdafileButton_->hide();
		if (directoryComboBoxmda_) directoryComboBoxmda_->hide();
		if (rhdfileLabel_) rhdfileLabel_->hide();
		if (rhdfileButton_) rhdfileButton_->hide();
		if (directoryComboBoxrhd_) directoryComboBoxrhd_->hide();
		if (convertOutputLabel_) convertOutputLabel_->hide();
		if (convertOutputButton_) convertOutputButton_->hide();
		if (directoryComboBoxconvert_) directoryComboBoxconvert_->hide();
		if (alignButton_) alignButton_->hide();
		if (convertToMda_) convertToMda_->hide();
		if (alignNeuralData_) alignNeuralData_->hide();
	}
	else
	{
		if (convertToMda_) convertToMda_->show();
		if (alignNeuralData_) alignNeuralData_->show();
	}

	if (alignNeuralData_)
		alignNeuralData_->setCheckState(Qt::Unchecked);
	if (convertToMda_)
		convertToMda_->setCheckState(Qt::Unchecked);

}
void ProxyMainWindow::convertToMDA()
{
	if (convertToMda_ && convertToMda_->isChecked())
	{
		if (alignNeuralData_)
			alignNeuralData_->setCheckState(Qt::Unchecked);

		if (sqlfileLabel_) sqlfileLabel_->hide();
		if (sqlfileButton_) sqlfileButton_->hide();
		if (directoryComboBoxsql_) directoryComboBoxsql_->hide();
		if (mdafileLabel_) mdafileLabel_->hide();
		if (mdafileButton_) mdafileButton_->hide();
		if (directoryComboBoxmda_) directoryComboBoxmda_->hide();
		if (rhdfileLabel_) rhdfileLabel_->show();
		if (rhdfileButton_) rhdfileButton_->show();
		if (directoryComboBoxrhd_) directoryComboBoxrhd_->show();
		if (convertOutputLabel_) convertOutputLabel_->show();
		if (convertOutputButton_) convertOutputButton_->show();
		if (directoryComboBoxconvert_) directoryComboBoxconvert_->show();
		if (alignButton_) alignButton_->show();

		convert_ = true;
		align_ = false;
	}
	else
	{
		convert_ = false;

		if (alignNeuralData_ && !alignNeuralData_->isChecked())
		{
			if (sqlfileLabel_) sqlfileLabel_->hide();
			if (sqlfileButton_) sqlfileButton_->hide();
			if (directoryComboBoxsql_) directoryComboBoxsql_->hide();
			if (mdafileLabel_) mdafileLabel_->hide();
			if (mdafileButton_) mdafileButton_->hide();
			if (directoryComboBoxmda_) directoryComboBoxmda_->hide();
		}
		if (rhdfileLabel_) rhdfileLabel_->hide();
		if (rhdfileButton_) rhdfileButton_->hide();
		if (directoryComboBoxrhd_) directoryComboBoxrhd_->hide();
		if (convertOutputLabel_) convertOutputLabel_->hide();
		if (convertOutputButton_) convertOutputButton_->hide();
		if (directoryComboBoxconvert_) directoryComboBoxconvert_->hide();
		if (alignButton_) alignButton_->hide();
	}


}
void ProxyMainWindow::alignNeuralData()
{
	if (alignNeuralData_ && alignNeuralData_->isChecked())
	{
		if (convertToMda_)
			convertToMda_->setCheckState(Qt::Unchecked);

		convert_ = false;
		align_ = true;

		if (sqlfileLabel_) sqlfileLabel_->show();
		if (sqlfileButton_) sqlfileButton_->show();
		if (directoryComboBoxsql_) directoryComboBoxsql_->show();
		if (mdafileLabel_) mdafileLabel_->show();
		if (mdafileButton_) mdafileButton_->show();
		if (directoryComboBoxmda_) directoryComboBoxmda_->show();
		if (rhdfileLabel_) rhdfileLabel_->show();
		if (rhdfileButton_) rhdfileButton_->show();
		if (directoryComboBoxrhd_) directoryComboBoxrhd_->show();
		if (convertOutputLabel_) convertOutputLabel_->hide();
		if (convertOutputButton_) convertOutputButton_->hide();
		if (directoryComboBoxconvert_) directoryComboBoxconvert_->hide();
		if (alignButton_) alignButton_->show();
	}
	else
	{
		align_ = false;

		if (sqlfileLabel_) sqlfileLabel_->hide();
		if (sqlfileButton_) sqlfileButton_->hide();
		if (directoryComboBoxsql_) directoryComboBoxsql_->hide();
		if (mdafileLabel_) mdafileLabel_->hide();
		if (mdafileButton_) mdafileButton_->hide();
		if (directoryComboBoxmda_) directoryComboBoxmda_->hide();

		if (convertToMda_ && !convertToMda_->isChecked())
		{
			if (rhdfileLabel_) rhdfileLabel_->hide();
			if (rhdfileButton_) rhdfileButton_->hide();
			if (directoryComboBoxrhd_) directoryComboBoxrhd_->hide();
			if (convertOutputLabel_) convertOutputLabel_->hide();
			if (convertOutputButton_) convertOutputButton_->hide();
			if (directoryComboBoxconvert_) directoryComboBoxconvert_->hide();
			if (alignButton_) alignButton_->hide();
		}
	}
}
void ProxyMainWindow::browsePictoFiles()
{
	QString directory = QFileDialog::getOpenFileName(this, tr("Find Files"), QDir::currentPath(), tr("SQL (*.sqlite)"));

	if (!directory.isEmpty()) {
		if (directoryComboBoxsql_->findText(directory) == -1)
			directoryComboBoxsql_->addItem(directory);
		directoryComboBoxsql_->setCurrentIndex(directoryComboBoxsql_->findText(directory));
	}
}
void ProxyMainWindow::browseMdaFiles()
{
	QString directory = QFileDialog::getOpenFileName(this, tr("Find Files"), QDir::currentPath(), tr("MDA (*.mda)"));

	if (!directory.isEmpty()) {
		if (directoryComboBoxmda_->findText(directory) == -1)
			directoryComboBoxmda_->addItem(directory);
		directoryComboBoxmda_->setCurrentIndex(directoryComboBoxmda_->findText(directory));
	}
}
void ProxyMainWindow::browserhdFiles()
{
	QString directory = QFileDialog::getOpenFileName(this, tr("Find Files"), QDir::currentPath(), tr("RHD (*.rhd)"));

	if (!directory.isEmpty()) {
		if (directoryComboBoxrhd_->findText(directory) == -1)
			directoryComboBoxrhd_->addItem(directory);
		directoryComboBoxrhd_->setCurrentIndex(directoryComboBoxrhd_->findText(directory));
	}
}
void ProxyMainWindow::browseSaveFolder()
{
	QString directory = QFileDialog::getExistingDirectory(this, tr("Select Output Folder"), QDir::currentPath());

	if (!directory.isEmpty()) {
		if (directoryComboBoxconvert_->findText(directory) == -1)
			directoryComboBoxconvert_->addItem(directory);
		directoryComboBoxconvert_->setCurrentIndex(directoryComboBoxconvert_->findText(directory));
	}
}
QComboBox *ProxyMainWindow::createIntanComboBox(const QString &text)
{
	QComboBox *comboBox = new QComboBox;
	comboBox->setEditable(true);
	comboBox->addItem(text);
	return comboBox;
}