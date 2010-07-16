#include "remoteviewer.h"

#include "../common/globals.h"
#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/engine/NetworkSignalChannel.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "../../common/storage/behavioraldatastore.h"
#include "../../common/statemachine/statemachine.h"



#include <QToolBar>
#include <QAction>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QComboBox>
#include <QLabel>
#include <QXmlStreamReader>
#include <QTimer>
#include <QSharedPointer>
#include <QMenu>


RemoteViewer::RemoteViewer(QWidget *parent) :
	Viewer(parent),
	status_(Stopped),
	serverChannel_(0),
	timeoutTimer_(0)
{
	setupServerChannel();
	setupEngine();
	setupUi();

	updateTimer_ = new QTimer(this);
	updateTimer_->setInterval(5000);
	connect(updateTimer_, SIGNAL(timeout()), this, SLOT(updateStatus()));
	connect(updateTimer_, SIGNAL(timeout()), this, SLOT(updateLists()));
	connect(updateTimer_, SIGNAL(timeout()), this, SLOT(updateActions()));
}

//! Called just before displaying the viewer
void RemoteViewer::init()
{
	if(serverChannel_ == 0 || 
		serverChannel_->getChannelStatus() == Picto::CommandChannel::disconnected)
	{
		QMessageBox msg;
		msg.setIcon(QMessageBox::Warning);
		msg.setText(Picto::Names->workstationAppName + tr(" is not connected to ") + Picto::Names->serverAppName + "\n"+
					   tr("Check your network connection, and ensure that ") + Picto::Names->serverAppName + tr(" is running."));
		msg.exec();
	}
	//If we're stopped, load the current experiment.  If we are paused,
	//then we shouldn't load the experiment.
	if(status_ == Stopped)
	{
		if(!experiment_)
		{
			QMessageBox msg;
			msg.setText("Failed to load current experiment.");
			msg.exec();

		}
	}

	generateTaskList();
	
	updateActions();
	updateLists();
	updateTimer_->start();
}

//!Called just before hiding the viewer
void RemoteViewer::deinit()
{
	//stop the engine running

	updateTimer_->stop();

}

//! Initializes the engine with all of the appropriate pieces for testing
void RemoteViewer::setupEngine()
{
	//set up the engine
	engine_ = QSharedPointer<Picto::Engine::PictoEngine>(new Picto::Engine::PictoEngine);
	engine_->setExclusiveMode(false);
	engine_->setSlaveMode(true,engineSlaveChannel_);

	//Set up the rendering target
	QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	pixmapVisualTarget_ = QSharedPointer<Picto::PixmapVisualTarget>(new Picto::PixmapVisualTarget(true,800,600));
	renderingTarget_ = QSharedPointer<Picto::RenderingTarget>(new Picto::RenderingTarget(pixmapVisualTarget_, pcmAuralTarget));
	engine_->addRenderingTarget(renderingTarget_);

	//Set up the visual target host
	//This exists because QSharedPointer<QWidget> results in multiple delete call, which 
	//gives us memory exceptions.
	visualTargetHost_ = new Picto::VisualTargetHost;
	visualTargetHost_->setVisualTarget(pixmapVisualTarget_);

	//set up signal channel
	QSharedPointer<Picto::SignalChannel> signalChannel(new Picto::NetworkSignalChannel(behavioralDataChannel_));
	engine_->addSignalChannel("PositionChannel",signalChannel);

	//Set up event code generator
	QSharedPointer<Picto::EventCodeGenerator> nullGenerator;
	nullGenerator = QSharedPointer<Picto::EventCodeGenerator>(new Picto::NullEventCodeGenerator());
	engine_->setEventCodeGenerator(nullGenerator);

	//set up reward controller
	QSharedPointer<Picto::RewardController> rewardController;
	rewardController = QSharedPointer<Picto::RewardController>(new Picto::AudioRewardController());
	engine_->setRewardController(rewardController);

	renderingTarget_->showSplash();
}

//! Sets up the user interface portions of the GUI
void RemoteViewer::setupUi()
{
	//----- Task Action -----
	playAction_ = new QAction(tr("P&lay"),this);
	playAction_->setIcon(QIcon(":/icons/play.png"));
	connect(playAction_,SIGNAL(triggered()),this, SLOT(play()));

	pauseAction_ = new QAction(tr("&Pause"),this);
	pauseAction_->setIcon(QIcon(":/icons/pause.png"));
	connect(pauseAction_,SIGNAL(triggered()),this, SLOT(pause()));

	stopAction_ = new QAction(tr("&Stop"),this);
	stopAction_->setIcon(QIcon(":/icons/stop.png"));
	connect(stopAction_,SIGNAL(triggered()),this, SLOT(stop()));

	rewardAction_ = new QAction(tr("&Reward"), this);
	rewardAction_->setIcon(QIcon(":/icons/reward.png"));
	connect(rewardAction_, SIGNAL(triggered()),this, SLOT(reward()));
	rewardChannel_ = 1;

	//TaskList combo box
	taskListBox_ = new QComboBox;
	taskListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	generateTaskList();

	//The toolbar
	toolBar_ = new QToolBar;

	toolBar_->addAction(playAction_);
	toolBar_->addAction(pauseAction_);
	toolBar_->addAction(stopAction_);
	toolBar_->addAction(rewardAction_);
	toolBar_->addSeparator();
	toolBar_->addWidget(new QLabel("Task: ", this));
	toolBar_->addWidget(taskListBox_);
	toolBar_->addSeparator();

	//----- Connection Actions -----
	connectAction_ = new QAction(tr("&Connect"), this);
	connectAction_->setCheckable(true);
	connectAction_->setIcon(QIcon(":/icons/disconnected.png"));
	connect(connectAction_, SIGNAL(toggled(bool)), this, SLOT(changeConnectionState(bool)));


	directorListBox_ = new QComboBox;
	directorListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	proxyListBox_ = new QComboBox;
	proxyListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	proxyListBox_->addItem("No Proxy",-1);
	updateLists();

	toolBar_->addAction(connectAction_);
	toolBar_->addWidget(new QLabel(Picto::Names->directorAppName + ": ", this));
	toolBar_->addWidget(directorListBox_);
	toolBar_->addWidget(new QLabel(Picto::Names->proxyServerAppName + ": ", this));
	toolBar_->addWidget(proxyListBox_);
	
	//------ Main layout -----------
	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(toolBar_);
	toolbarLayout->addStretch();

	statusBar_ = new QLabel();
	updateStatus();

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	mainLayout->addWidget(visualTargetHost_);
	mainLayout->addStretch(1);
	mainLayout->addWidget(statusBar_);
	setLayout(mainLayout);

	updateActions();

}

void RemoteViewer::setupServerChannel()
{
	serverChannel_ = new Picto::CommandChannel(this);
	engineSlaveChannel_ = new Picto::CommandChannel(this);
	behavioralDataChannel_ = new Picto::CommandChannel(this);

	serverDiscoverer_ = new Picto::ServerDiscoverer(this);

	connect(serverDiscoverer_, SIGNAL(foundServer(QHostAddress, quint16)), serverChannel_, SLOT(connectToServer(QHostAddress, quint16)));
	connect(serverDiscoverer_, SIGNAL(foundServer(QHostAddress, quint16)), engineSlaveChannel_, SLOT(connectToServer(QHostAddress, quint16)));
	connect(serverDiscoverer_, SIGNAL(foundServer(QHostAddress, quint16)), behavioralDataChannel_, SLOT(connectToServer(QHostAddress, quint16)));

	connect(serverDiscoverer_, SIGNAL(discoverFailed()), serverDiscoverer_, SLOT(discover()));

	serverDiscoverer_->discover(30000);
}


void RemoteViewer::play()
{
	qDebug()<<"Play slot";

	if(status_ == Stopped)
	{
		QString modifiedTaskName = taskListBox_->currentText();
		modifiedTaskName = modifiedTaskName.simplified().remove(' ');

		if(sendTaskCommand("start:"+modifiedTaskName))
		{
			status_ = Running;
			updateActions();
			if(experiment_)
			{
				experiment_->runTask(modifiedTaskName, engine_);
				status_ = Stopped;
				updateActions();
				renderingTarget_->showSplash();
			}
		}
	}
	else if(status_ == Paused)
	{
		if(sendTaskCommand("resume"))
		{
			engine_->resume();
			status_ = Running;
			updateActions();
		}
	}
	updateActions();
}

void RemoteViewer::pause()
{
	if(sendTaskCommand("pause"))
	{
		status_ = Paused;
		updateActions();
		engine_->pause();
	}
	updateActions();
}

void RemoteViewer::stop()
{
	Q_ASSERT(sendTaskCommand("stop"));
	status_ = Stopped;
	updateActions();
	engine_->stop();
}

void RemoteViewer::reward()
{
	if(sendTaskCommand(QString("reward:%1").arg(rewardChannel_)))
	{
		engine_->giveReward(rewardChannel_);
	}
}

//! \brief Called whenever the connected button changes state
void RemoteViewer::changeConnectionState(bool checked)
{
	if(checked)
	{
	
		QString directorAddr = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
		if(directorIsRunning(directorAddr))
		{
			//! \TODO Handle joining a running session...
			Q_ASSERT(false);
		}
		else
		{
			if(startSession())
			{
				connectAction_->setIcon(QIcon(":/icons/connected.png"));
				startedSession_ = true;
			}
			else
			{
				connectAction_->setChecked(false);
			}
		}
	}
	else
	{

		if(startedSession_)
		{
			//We started the session, so disconnecting from it will kill it.
			if(endSession())
			{
				connectAction_->setIcon(QIcon(":/icons/disconnected.png"));
			}
			else
				connectAction_->setChecked(true);
		}
		else
		{
			//If we joined an already active sesssion, we aren't allowed to kill it
		}


	}
	updateActions();
}


//! \brief Updates the status bar (called by a 5 sec timer)
void RemoteViewer::updateStatus()
{
	QString statusLine;

	//Check server connection
	//I'm going to assume that if one of our signal channels is connected that they
	//all are.  this may prove to be a faulty assumption, but I doubt it...
	if(serverChannel_->isConnected())
		statusLine += tr("Server: connected");
	else
		statusLine += tr("Server: disconnected");

	if(!sessionId_.isNull())
		statusLine += tr("    Session Id: ") + sessionId_.toString();

	statusBar_->setText(statusLine);
}

//! \brief Sets the status message to the passed in string for 5 seconds
void RemoteViewer::setStatus(QString status)
{
	statusBar_->setText(status);
	updateTimer_->start();	//This gaurantees that the message stays up for 5 seconds
}

void RemoteViewer::generateTaskList()
{
	Q_ASSERT(taskListBox_);
	taskListBox_->clear();

	if(!experiment_)
		return;

	taskListBox_->addItems(experiment_->getTaskNames());
	
	taskListBox_->setEnabled(taskListBox_->count() > 0);

	updateActions();
}

/*!	\brief Updates the status of the actions
 *
 *	This viewer has a lot of different actions, so this centralized function
 *	handles the process of enabling and disabling them all.  It should be called
 *	directly, whenever a state change is made.
 *
 *	Be careful when modifying this function.  It is easy to make logic errors.
 *	To reduce the chances of logic errors, each action is enabled/disabled by
 *	a single disable boolean.
 */
void RemoteViewer::updateActions()
{
	//Play action
	bool disablePlay = false;
	disablePlay |= !serverChannel_->isConnected();
	disablePlay |= !connectAction_->isChecked();
	disablePlay |= (taskListBox_->count() == 0);
	disablePlay |= (directorListBox_->count() == 0);
	disablePlay |= (status_ == Running);
	disablePlay |= experiment_.isNull();

	if(disablePlay)
		playAction_->setEnabled(false);
	else
		playAction_->setEnabled(true);

	//Stop action
	bool disableStop = false;
	disableStop |= !(status_ == Running || status_ == Paused);
	disableStop |= !connectAction_->isChecked();

	if(disableStop)
		stopAction_->setEnabled(false);
	else
		stopAction_->setEnabled(true);


	//Pause action
	bool disablePause = false;
	disablePause |= (status_ != Running);
	disablePause |= !connectAction_->isChecked();

	if(disablePause)
		pauseAction_->setEnabled(false);
	else
		pauseAction_->setEnabled(true);

	//Reward action
	bool disableReward = false;
	disableReward |= !connectAction_->isChecked();

	if(disableReward)
		rewardAction_->setEnabled(false);
	else
		rewardAction_->setEnabled(true);

	//Connect Action
	bool disableConnect = false;
	disableConnect |= (directorListBox_->count() == 0);
	disableConnect |= (proxyListBox_->count() == 0);

	if(disableConnect)
		connectAction_->setEnabled(false);
	else
		connectAction_->setEnabled(true);

	//Director and Proxy combo boxes
	bool disableDirectorAndProxyLists = false;
	disableDirectorAndProxyLists |= (connectAction_->isChecked());

	if(disableDirectorAndProxyLists)
	{
		directorListBox_->setEnabled(false);
		proxyListBox_->setEnabled(false);
	}
	else
	{
		directorListBox_->setEnabled(true);
		proxyListBox_->setEnabled(true);
	}



}


/*! \brief updates the combo boxes containing Director instances and proxy servers
 *
 *	This function asks the server for a current list of Director instances, and 
 *	then adds new ones to the combo box and removes old ones, as needed.  The same
 *	is done for the proxy servers
 */
void RemoteViewer::updateLists()
{
	if(connectAction_->isChecked())
	{
		directorListBox_->setEnabled(false);
		proxyListBox_->setEnabled(false);
		return;
	}
 
	//Update the director combo box
	QList<DirectorInstance> directors;
	directors = getDirectorList();

	//Add new directors
	for(int i=0; i<directors.length(); i++)
	{
		DirectorInstance d = directors[i];
		if(directorListBox_->findData(d.address) == -1)
			directorListBox_->addItem(d.name, d.address);
	}

	//Remove old directors
	for(int i=0; i<directorListBox_->count(); i++)
	{
		bool remove = true;
		for(int j=0; j<directors.length(); j++)
		{
			if(directors[j].address ==directorListBox_->itemData(i))
			{
				remove = false;
				break;
			}
		}
		if(remove)
			directorListBox_->removeItem(i);
	}

	//Update the proxy combo box
	QList<ProxyServerInfo> proxies = getProxyList();

	//Add new proxies
	for(int i=0; i<proxies.length(); i++)
	{
		ProxyServerInfo proxy = proxies[i];
		if(proxyListBox_->findData(proxy.id) == -1)
			proxyListBox_->addItem(proxy.name, proxy.id);
	}

	//Remove old proxies
	//(We're skipping the first entry, since that's "No Proxy")
	for(int i=1; i<proxyListBox_->count(); i++)
	{
		bool remove = true;
		for(int j=0; j<proxies.length(); j++)
		{
			if(proxies[j].id ==proxyListBox_->itemData(i))
			{
				remove = false;
				break;
			}
		}
		if(remove)
			proxyListBox_->removeItem(i);
	}

	updateActions();
}

/*! \brief returns true if the director is currently running a task
 *
 *	Since we do things differently if we are conecting to a Diretor instance that is
 *	already running a task, we need to check before connecting.
 */
bool RemoteViewer::directorIsRunning(QString addr)
{
	QList<DirectorInstance> directors = getDirectorList();

	foreach(DirectorInstance director, directors)
	{
		if(director.address == addr)
		{
			if(director.status.toUpper() == "IDLE")
				return false;
			else
				return true;
		}
	}
	//If we made it this far, something is wrong...
	Q_ASSERT_X(false, "RemoteViewer::directorIsRunning", "Passed in address not an active director instance");
	return false;
}

/*! \brief Returns a list of directors and their properties
 *
 *	This sends out a DIRECTORLIST command, and then parses the response into a 
 *	QList of DirectorInstances (really just a struct with a couple of strings).
 *	If something does wrong, or no director instances are found, an empy
 *	list is returned.
 */  
QList<RemoteViewer::DirectorInstance> RemoteViewer::getDirectorList()
{
	Q_ASSERT(serverChannel_->incomingResponsesWaiting() == 0);
	QList<DirectorInstance> directors;

	if(!serverChannel_->isConnected())
	{
		return directors;
	}

	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("DIRECTORLIST / PICTO/1.0"));
	QSharedPointer<Picto::ProtocolResponse> response;

	serverChannel_->sendCommand(command);
	if(!serverChannel_->waitForResponse(1000))
	{
		setStatus(tr("Server did not respond to DIRECTORLIST command"));
		return directors;
	}
	response = serverChannel_->getResponse();
	
	QByteArray xmlFragment = response->getContent();
	QXmlStreamReader xmlReader(xmlFragment);

	while(!xmlReader.atEnd())
	{
		xmlReader.readNext();

		if(xmlReader.isStartElement() && xmlReader.name() == "Director")
		{
			DirectorInstance director;

			while(!xmlReader.atEnd())
			{
				xmlReader.readNext();

				if(xmlReader.name() == "Name" && xmlReader.isStartElement())
				{
					director.name = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Address" && xmlReader.isStartElement())
				{
					director.address = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Status" && xmlReader.isStartElement())
				{
					director.status = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Director" && xmlReader.isEndElement())
				{
					directors.append(director);
					break;
				}
			}
		}
	}
	return directors;
}

/*! \brief Returns a list of proxy servers and their address
 *
 *	This sends out a PROXYLIST command, and then parses the response into a 
 *	QList of ProxyServerInfos (really just a struct with the name and ID number
 *	for each proxy).
 *	
 */  
QList<RemoteViewer::ProxyServerInfo> RemoteViewer::getProxyList()
{
	QList<ProxyServerInfo> proxies;

	if(!serverChannel_->isConnected())
	{
		return proxies;
	}

	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("PROXYLIST / PICTO/1.0"));
	QSharedPointer<Picto::ProtocolResponse> response;

	serverChannel_->sendCommand(command);
	if(!serverChannel_->waitForResponse(1000))
	{
		setStatus(tr("Server did not respond to PROXYLIST command"));
		return proxies;
	}
	response = serverChannel_->getResponse();
	
	QByteArray xmlFragment = response->getContent();
	QXmlStreamReader xmlReader(xmlFragment);

	while(!xmlReader.atEnd())
	{
		xmlReader.readNext();

		if(xmlReader.isStartElement() && xmlReader.name() == "Proxy")
		{
			ProxyServerInfo proxy;

			while(!xmlReader.atEnd())
			{
				xmlReader.readNext();

				if(xmlReader.name() == "Name" && xmlReader.isStartElement())
				{
					proxy.name = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Id" && xmlReader.isStartElement())
				{
					proxy.id = xmlReader.readElementText().toInt();
				}
				else if(xmlReader.name() == "Proxy" && xmlReader.isEndElement())
				{
					proxies.append(proxy);
					break;
				}
				else
				{
					//There shouldn't be anything else in here...
					Q_ASSERT(false);
				}
			}
		}
	}

	return proxies;
}

/*! \brief checks to see if the current proxy or director has timed out
 *
 *	When we are in a session, this slot is called periodically, to confirm that the
 *	proxy and director being used haven't disapearred from the network.
 */
void RemoteViewer::checkForTimeouts()
{
	//Check the director
	QList<DirectorInstance> directors =  getDirectorList();

	bool foundDirector = false;
	foreach(DirectorInstance director, directors)
	{
		QString testName = directorListBox_->currentText();
		QString testAddr = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
		if(director.name == directorListBox_->currentText() &&
			director.address == directorListBox_->itemData(directorListBox_->currentIndex()).toString())
		{
			foundDirector = true;
			break;
		}
	}

	if(!foundDirector)
	{
		//connectAction_->setChecked(false);
		//changeConnectionState(false);
		connectAction_->setChecked(false);
		QMessageBox::critical(0,tr("Director Lost"),
			tr("The director instance being used is no longer on the network." 
			"The session has been ended."));
		return;
	}

	//Check the proxy
	if(proxyListBox_->itemData(proxyListBox_->currentIndex()).toInt() == -1)
		return;
	QList<ProxyServerInfo> proxies = getProxyList();
	
	bool foundProxy = false;
	foreach(ProxyServerInfo proxy, proxies)
	{
		if(proxy.name == proxyListBox_->currentText() &&
			proxy.id == proxyListBox_->itemData(proxyListBox_->currentIndex()).toInt())
		{
			foundProxy = true;
			break;
		}
	}
	if(!foundProxy)
	{
		connectAction_->setChecked(false);
		QMessageBox::critical(0,tr("Proxy Lost"),
			tr("The proxy server being used is no longer on the network." 
			"The session has been ended."));
		return;
	}

}

//Starts a new session
bool RemoteViewer::startSession()
{
	//Check for a currently running session
	if(!sessionId_.isNull())
		return true;

	QString commandStr;
	QString directorAddr = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
	int proxyId = proxyListBox_->itemData(proxyListBox_->currentIndex()).toInt();
	commandStr = "STARTSESSION "+directorAddr+"/"+QString::number(proxyId)+" PICTO/1.0";

	QSharedPointer<Picto::ProtocolCommand> loadExpCommand(new Picto::ProtocolCommand(commandStr));

	QByteArray experimentXml = experimentText_->toPlainText().toUtf8();
	loadExpCommand->setContent(experimentXml);
	loadExpCommand->setFieldValue("Content-Length",QString("%1").arg(experimentXml.length()));

	QSharedPointer<Picto::ProtocolResponse> loadExpResponse;

	serverChannel_->sendCommand(loadExpCommand);
	if(!serverChannel_->waitForResponse(5000))
	{
		setStatus(tr("Server did not respond to STARTSESSION command"));
		return false;
	}

	loadExpResponse = serverChannel_->getResponse();
	if(loadExpResponse.isNull())
	{
		setStatus(tr("No response received from server.\nExperiment not loaded."));
		return false;
	}
	else if(loadExpResponse->getResponseCode() == Picto::ProtocolResponseType::NotFound)
	{
		setStatus(tr("Director instance not found at:") + directorAddr +tr(".  Experiment not loaded."));
		return false;
	}
	else if(loadExpResponse->getResponseCode() == Picto::ProtocolResponseType::Unauthorized)
	{
		setStatus(tr("Director instance has already loaded an experiment.  Experiment not loaded."));
		return false;
	}
	else if(loadExpResponse->getResponseCode() == Picto::ProtocolResponseType::OK)
	{

		QByteArray content = loadExpResponse->getDecodedContent();
		QXmlStreamReader xmlReader(content);

		while(!xmlReader.atEnd() && !(xmlReader.isStartElement() && xmlReader.name() == "SessionID"))
			xmlReader.readNext();

		if(!xmlReader.atEnd())
		{
			sessionId_ = QUuid(xmlReader.readElementText());
			serverChannel_->setSessionId(sessionId_);
			engineSlaveChannel_->setSessionId(sessionId_);
			behavioralDataChannel_->setSessionId(sessionId_);
			setStatus(tr("Experiment loaded on remote Director instance. Session ID: ")+ sessionId_.toString());
			
			//This resets the timing on all of our slave elements.  Note that this will eventually
			//need to be rebuilt, because in the current arrangement, we can only have one running 
			//slave state machine
			Picto::StateMachine::resetSlaveElements();

			//Start the timeout timer
			if(!timeoutTimer_)
			{
				timeoutTimer_ = new QTimer(this);
				timeoutTimer_->setInterval(500);
				connect(timeoutTimer_, SIGNAL(timeout()), this, SLOT(checkForTimeouts()));
			}
			timeoutTimer_->start();

			return true;
		}
		else
		{
			setStatus(tr("SessionID not found in response from server."));
			return false;
		}
	}
	else
	{
		setStatus(tr("Unexpected response from server.\nExperiment not loaded."));
		return false;
	}

}

//! Ends a currently running session
bool RemoteViewer::endSession()
{
	//Check for a currently running session
	if(sessionId_.isNull())
	{
		setStatus(tr("Not currently in a session"));
	}

	//If we're running, we should stop first
	if(status_ == Running || status_ == Paused)
	{
		stop();

		//wait for the remote engine to stop
		QString directorAddr = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
		while(directorIsRunning(directorAddr));
	}

	QSharedPointer<Picto::ProtocolCommand> endSessCommand(
		new Picto::ProtocolCommand("ENDSESSION "+sessionId_+" PICTO/1.0"));
	QSharedPointer<Picto::ProtocolResponse> endSessResponse;

	serverChannel_->sendCommand(endSessCommand);
	if(!serverChannel_->waitForResponse(5000))
	{
		setStatus(tr("Server did not respond to ENDSESSION command"));
		qDebug()<<(tr("Server did not respond to ENDSESSION command"));
	}

	endSessResponse = serverChannel_->getResponse();
	if(endSessResponse.isNull())
	{
		setStatus(tr("No response received from server.\nExperiment not loaded."));
	}
	else if(endSessResponse->getResponseCode() == Picto::ProtocolResponseType::NotFound)
	{
		setStatus(tr("Session ID not found, session not ended"));
	}
	else if(endSessResponse->getResponseCode() == Picto::ProtocolResponseType::OK)
	{
		setStatus(tr("Session ended"));
	}
	else
	{
		setStatus(tr("Unexpected response to ENDSESSION command"));
	}
	status_ = Stopped;
	serverChannel_->setSessionId(QUuid());
	engineSlaveChannel_->setSessionId(QUuid());
	behavioralDataChannel_->setSessionId(QUuid());
	sessionId_ = QUuid();
	timeoutTimer_->stop();
	return true;

}

//! Sends a task command with the given target.  Returns true if execution was successful
bool RemoteViewer::sendTaskCommand(QString target)
{
	QSharedPointer<Picto::ProtocolCommand> cmd(new Picto::ProtocolCommand("TASK "+target+" PICTO/1.0"));
	
	QSharedPointer<Picto::ProtocolResponse> cmdResponse;

	serverChannel_->sendCommand(cmd);
	if(!serverChannel_->waitForResponse(5000))
	{
		setStatus(tr("Server did not respond to commmand: TASK ")+target);
		return false;
	}

	cmdResponse = serverChannel_->getResponse();

	if(cmdResponse->getResponseType() == "OK")
	{
		setStatus("TASK "+target+tr(" command completed"));
		return true;
	}
	else if(cmdResponse->getResponseCode() == 400)
	{
		setStatus("TASK "+target+tr(" command failed: Unreasonable request"));
		return false;
	}
	else if(cmdResponse->getResponseCode() == 404)
	{
		setStatus("TASK "+target+tr(" command failed: Session ID not recognized"));
		return false;
	}
	else
	{
		Q_ASSERT(false);
		return false;
	}
}