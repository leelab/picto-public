#include "remoteviewer.h"

#include "../common/globals.h"
#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/engine/NetworkSignalChannel.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "../../common/storage/BehavioralDataUnitPackage.h"
#include "../../common/storage/StateDataUnit.h"
#include "../../common/statemachine/statemachine.h"
#include "../propertyframe.h"



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
#include <QTime>
#include <QSharedPointer>
#include <QMenu>
#include <QCloseEvent>
#include <QSqlDatabase>
#include <QSqlQuery>


RemoteViewer::RemoteViewer(QWidget *parent) :
	Viewer(parent),
	localStatus_(Stopped),
	serverChannel_(0),
	timeoutTimer_(0),
	startedSession_(false),
	activeExperiment_(0)
{
	setupServerChannel();
	setupEngine();
	setupUi();

	updateTimer_ = new QTimer(this);
	updateTimer_->setInterval(5000);
	connect(updateTimer_, SIGNAL(timeout()), this, SLOT(updateStatus()));
	connect(updateTimer_, SIGNAL(timeout()), this, SLOT(updateLists()));
	connect(updateTimer_, SIGNAL(timeout()), this, SLOT(updateActions()));








	QString dbName = "PictoWorkstation";
	dbName = dbName.toLower();
	QSqlDatabase configDb = QSqlDatabase::addDatabase("QSQLITE",dbName);
	configDb.setDatabaseName(QCoreApplication::applicationDirPath() + "/" + dbName + ".config");
	configDb.open();

	QSqlQuery query(configDb);
	if(!configDb.tables().contains("workstationinfo"))
	{
		query.exec("CREATE TABLE workstationinfo (key TEXT, value TEXT)");
		observerId_ = QUuid::createUuid();
		query.prepare("INSERT INTO workstationinfo (key,value) VALUES ('id',:id)");
		query.bindValue(":id",QString(observerId_));
		query.exec();
	}
	query.exec("SELECT value FROM workstationinfo WHERE key='id'");
	bool rc = query.next();
	Q_ASSERT(rc);
	observerId_ = QUuid(query.value(0).toString());
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
	if(sessionId_ == QUuid())
	{
		//Only load a new experiment if we're not already in a session
		experiment_ = pictoData_->getExperiment();
		if(!experiment_)
		{
			QMessageBox msg;
			msg.setText("Failed to load current experiment.");
			msg.exec();

		}
		else
		{
			static_cast<PropertyFrame*>(propertyFrame_)->setTopLevelDataStore(experiment_.staticCast<DataStore>());
		}

	}
	

	//If we're stopped, load the current experiment.  If we are paused,
	//then we shouldn't load the experiment.
	/*if(localStatus_ == Stopped)
	{
		if(!experiment_)
		{
			QMessageBox msg;
			msg.setText("Failed to load current experiment.");
			msg.exec();

		}
	}*/

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


bool RemoteViewer::aboutToQuit()
{
	if(!connectAction_->isChecked())
		return true;

	//if(!startedSession_)
	//{
	//	disjoinSession();
	//	return true;
	//}
	
	changeConnectionState(false);
	return true;

	//int r = QMessageBox::warning(this,Picto::Names->workstationAppName,
	//			tr("There is an active session.\nQuitting will end the session\n"
	//			"Do you still want to quit?"),
	//			   QMessageBox::Yes|QMessageBox::No);
	//if(r == QMessageBox::Yes)
	//{
	//	changeConnectionState(false);
	//	return true;
	//}
	//else
	//	return false;	
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
	connect(connectAction_, SIGNAL(triggered(bool)), this, SLOT(changeConnectionState(bool)));


	directorListBox_ = new QComboBox;
	directorListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	proxyListBox_ = new QComboBox;
	proxyListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	proxyListBox_->addItem("No Proxy",QUuid().toString());
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

	QHBoxLayout *operationLayout = new QHBoxLayout;
	propertyFrame_ = new PropertyFrame();
	connect(taskListBox_,SIGNAL(currentIndexChanged(int)),this,SLOT(taskListIndexChanged(int)));
	operationLayout->addWidget(propertyFrame_,Qt::AlignTop);
	operationLayout->addWidget(visualTargetHost_);
	operationLayout->setStretch(0,0);
	operationLayout->addStretch();

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	mainLayout->addLayout(operationLayout);
	mainLayout->addStretch(1);
	mainLayout->addWidget(statusBar_);
	setLayout(mainLayout);

	updateActions();

}

void RemoteViewer::setupServerChannel()
{
	serverChannel_ = new Picto::CommandChannel(observerId_,"WORKSTATION",this);
	engineSlaveChannel_ = new Picto::CommandChannel(observerId_,"WORKSTATION",this);
	behavioralDataChannel_ = new Picto::CommandChannel(observerId_,"WORKSTATION",this);

	serverDiscoverer_ = new Picto::ServerDiscoverer(this);

	connect(serverDiscoverer_, SIGNAL(foundServer(QHostAddress, quint16)), serverChannel_, SLOT(connectToServer(QHostAddress, quint16)));
	connect(serverDiscoverer_, SIGNAL(foundServer(QHostAddress, quint16)), engineSlaveChannel_, SLOT(connectToServer(QHostAddress, quint16)));
	connect(serverDiscoverer_, SIGNAL(foundServer(QHostAddress, quint16)), behavioralDataChannel_, SLOT(connectToServer(QHostAddress, quint16)));

	connect(serverChannel_, SIGNAL(connectAttemptFailed()), serverDiscoverer_, SLOT(discover()));
	connect(engineSlaveChannel_, SIGNAL(connectAttemptFailed()), serverDiscoverer_, SLOT(discover()));
	connect(behavioralDataChannel_, SIGNAL(connectAttemptFailed()), serverDiscoverer_, SLOT(discover()));

	connect(serverChannel_, SIGNAL(channelDisconnected()), serverDiscoverer_, SLOT(discover()));
	connect(engineSlaveChannel_, SIGNAL(channelDisconnected()), serverDiscoverer_, SLOT(discover()));
	connect(behavioralDataChannel_, SIGNAL(channelDisconnected()), serverDiscoverer_, SLOT(discover()));

	connect(serverDiscoverer_, SIGNAL(discoverFailed()), serverDiscoverer_, SLOT(discover()));

	serverDiscoverer_->discover(30000);
}


void RemoteViewer::play()
{
	qDebug()<<"Play slot";

	timeoutTimer_->stop();
	if(localStatus_ == Stopped)
	{
		QString modifiedTaskName = taskListBox_->currentText();
		modifiedTaskName = modifiedTaskName.simplified().remove(' ');

		if(sendTaskCommand("start:"+modifiedTaskName))
		{
			//Once we've sent the start command, we can disjoin and then rejoin the session
			//This gaurantees that our state machine is in the corerct state.
			disjoinSession();
			joinSession();
		}
	}
	else if(localStatus_ == Paused)
	{
		if(sendTaskCommand("resume"))
		{
			engine_->resume();
			localStatus_ = Running;
			updateActions();
		}
	}
	updateActions();
	timeoutTimer_->start();
}

void RemoteViewer::pause()
{
	timeoutTimer_->stop();

	if(sendTaskCommand("pause"))
	{
		localStatus_ = Paused;
		engine_->pause();
	}
	updateActions();
	timeoutTimer_->start();
}

void RemoteViewer::stop()
{
	timeoutTimer_->stop();
	sendTaskCommand("stop");
	localStatus_ = Stopped;
	engine_->stop();
	updateActions();
	timeoutTimer_->start();
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
		QString directorID = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
		//We need to possibbly start a session, and always join a session
		ComponentStatus remoteStatus = directorStatus(directorID);
		if(remoteStatus == NotFound)
		{
			connectAction_->setChecked(false);
			return;
		}

		//If the remote Director is not already in a session, we must start a new one
		if(remoteStatus == Idle)
		{
			if(startSession())
			{
				connectAction_->setIcon(QIcon(":/icons/connected.png"));
				startedSession_ = true;
			}
			else
			{
				connectAction_->setChecked(false);
				return;
			}
		}

		//We always need to join the session

		//joinSession doesn't return until the session ends (this could be fixed
		//by using a signal to run the task, but  tasks were designed to be run in 
		//an environment without event loops).  This means that we need to handle
		//everything before calling it.
		connectAction_->setIcon(QIcon(":/icons/connected.png"));
		if(!joinSession())
		{
			connectAction_->setChecked(false);
			connectAction_->setIcon(QIcon(":/icons/disconnected.png"));
		}
	}
	else
	{ 
		QString directorID = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
		ComponentStatus remoteStatus = directorStatus(directorID);
		int r = QMessageBox::No;
		if((remoteStatus == Stopped) && (startedSession_))
		{
			r = QMessageBox::warning(this,Picto::Names->workstationAppName,
				tr("You are disconnecting from a session that is not currently running an experiment.\n"
				"Would you like to end the session as well?"),
				   QMessageBox::Yes|QMessageBox::No);
		}

		//If we're running, we should stop first
		if(r == QMessageBox::Yes)
		{
			stop();

			//wait for the remote engine to stop
			QString directorID = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
			QTime timer;
			timer.start();
			while(directorStatus(directorID) > Stopped && timer.elapsed() < 2000);

			Q_ASSERT_X(timer.elapsed()<2000, "RemoteViewer::changeConnectionState", "Remote director never stopped");
		}
		else
		{
			engine_->stop();
			localStatus_= Stopped;
			updateActions();
		}
		
		//If we started the session, we should end it too
		if(r == QMessageBox::Yes)
		{
			if(!endSession())
			{
				QMessageBox::critical(0,tr("Failed to End Session"),
				tr("Failed to end the session.  Please reconnect to the session and try again."));
			}
		}

		//We should disjoin the session first (this will alwys occur
		if(disjoinSession())
			connectAction_->setIcon(QIcon(":/icons/disconnected.png"));
		else
			connectAction_->setChecked(true);

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
	if(serverChannel_->isConnected() &&
	   engineSlaveChannel_->isConnected() &&
	   behavioralDataChannel_->isConnected())
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

	if(activeExperiment_)
		taskListBox_->addItems(activeExperiment_->getTaskNames());
	else if(experiment_)
		taskListBox_->addItems(experiment_->getTaskNames());
	else
		return;
	
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
	if(startedSession_)
		enableTaskCommands_ = true;

	//Play action
	bool disablePlay = false;
	disablePlay |= !enableTaskCommands_;
	disablePlay |= !serverChannel_->isConnected();
	disablePlay |= !connectAction_->isChecked();
	disablePlay |= (taskListBox_->count() == 0);
	disablePlay |= (directorListBox_->count() == 0);
	disablePlay |= (localStatus_ == Running);
	disablePlay |= experiment_.isNull();

	if(disablePlay)
		playAction_->setEnabled(false);
	else
		playAction_->setEnabled(true);

	//Stop action
	bool disableStop = false;
	disableStop |= !enableTaskCommands_;
	disableStop |= !(localStatus_ == Running || localStatus_ == Paused);
	disableStop |= !connectAction_->isChecked();

	if(disableStop)
		stopAction_->setEnabled(false);
	else
		stopAction_->setEnabled(true);


	//Pause action
	bool disablePause = false;
	disablePause |= !enableTaskCommands_;
	disablePause |= (localStatus_ != Running);
	disablePause |= !connectAction_->isChecked();

	if(disablePause)
		pauseAction_->setEnabled(false);
	else
		pauseAction_->setEnabled(true);

	//Reward action
	bool disableReward = false;
	disableReward |= !enableTaskCommands_;
	disableReward |= !connectAction_->isChecked();

	if(disableReward)
		rewardAction_->setEnabled(false);
	else
		rewardAction_->setEnabled(true);

	//Connect Action
	bool disableConnect = false;
	disableConnect |= (directorListBox_->count() == 0);

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

	//Task list combo box
	bool disableTaskList = false;
	disableTaskList |= !enableTaskCommands_;
	disableTaskList |= (localStatus_ == Running);
	disableTaskList |= (localStatus_ == Paused);

	if(disableTaskList)
		taskListBox_->setEnabled(false);
	else
		taskListBox_->setEnabled(true);


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

	QIcon runningIcon(QIcon(":/icons/runningWorkstation.png"));
	QIcon endingSession(QIcon(":/icons/endingSession.png"));

	//Update the director combo box
	QList<ComponentInstance> directors;
	directors = getDirectorList();

	//Add new directors
	for(int i=0; i<directors.length(); i++)
	{
		ComponentInstance d = directors[i];
		if(directorListBox_->findData(d.id) == -1)
		{
			if(d.status.toUpper() == "IDLE")
				directorListBox_->addItem(d.name, d.id);
			else if(d.status.toUpper() == "ENDING")
				directorListBox_->addItem(endingSession, d.name, d.id);
			else
				directorListBox_->addItem(runningIcon, d.name, d.id);
		}
	}

	//Remove old directors
	for(int i=0; i<directorListBox_->count(); i++)
	{
		bool remove = true;
		for(int j=0; j<directors.length(); j++)
		{
			if(directors[j].id ==directorListBox_->itemData(i))
			{
				if(directors[j].status.toUpper() == "IDLE")
					directorListBox_->setItemIcon(i,QIcon());
				else if(directors[j].status.toUpper() == "ENDING")
					directorListBox_->setItemIcon(i,endingSession);
				else
					directorListBox_->setItemIcon(i,runningIcon);

				remove = false;
				break;
			}
		}
		if(remove)
			directorListBox_->removeItem(i);
	}

	//Update the proxy combo box
	QList<ComponentInstance> proxies;
	proxies = getProxyList();

	//Add new proxies
	for(int i=0; i<proxies.length(); i++)
	{
		ComponentInstance p = proxies[i];
		if(proxyListBox_->findData(p.id) == -1)
		{
			if(p.status.toUpper() == "IDLE")
				proxyListBox_->addItem(p.name, p.id);
			else if(p.status.toUpper() == "ENDING")
				proxyListBox_->addItem(endingSession, p.name, p.id);
			else
				proxyListBox_->addItem(runningIcon, p.name, p.id);
		}
	}

	//Remove old proxies
	// Skip first entry because that's the "No Proxy" option.
	for(int i=1; i<proxyListBox_->count(); i++)
	{
		bool remove = true;
		for(int j=0; j<proxies.length(); j++)
		{
			if(proxies[j].id ==proxyListBox_->itemData(i))
			{
				if(proxies[j].status.toUpper() == "IDLE")
					proxyListBox_->setItemIcon(i,QIcon());
				else if(proxies[j].status.toUpper() == "ENDING")
					proxyListBox_->setItemIcon(i,endingSession);
				else
					proxyListBox_->setItemIcon(i,runningIcon);

				remove = false;
				break;
			}
		}
		if(remove)
			proxyListBox_->removeItem(i);
	}

	updateActions();
}

/*! \brief returns the current status of a remote director
 */
RemoteViewer::ComponentStatus RemoteViewer::directorStatus(QString id)
{
	QList<ComponentInstance> directors = getDirectorList();

	foreach(ComponentInstance director, directors)
	{
		if(director.id == id)
		{
			if(director.status.toUpper() == "IDLE")
				return Idle;
			if(director.status.toUpper() == "ENDING")
				return Ending;
			else if(director.status.toUpper() == "STOPPED")
				return Stopped;
			else if(director.status.toUpper() == "RUNNING")
				return Running;
			else if(director.status.toUpper() == "PAUSED")
				return Paused;
			else
				return NotFound;
		}
	}
	//If we made it this far, something is wrong...
	return NotFound;
}

/*! \brief returns the current status of a remote proxy
 */
RemoteViewer::ComponentStatus RemoteViewer::proxyStatus(QString id)
{
	QList<ComponentInstance> proxies = getProxyList();

	foreach(ComponentInstance proxy, proxies)
	{
		if(proxy.id == id)
		{
			if(proxy.status.toUpper() == "IDLE")
				return Idle;
			if(proxy.status.toUpper() == "ENDING")
				return Ending;
			else if(proxy.status.toUpper() == "STOPPED")
				return Stopped;
			else if(proxy.status.toUpper() == "RUNNING")
				return Running;
			else if(proxy.status.toUpper() == "PAUSED")
				return Paused;
			else
				return NotFound;
		}
	}
	//If we made it this far, something is wrong...
	return NotFound;
}

/*! \brief Returns a list of directors and their properties
 *
 *	This sends out a DIRECTORLIST command, and then parses the response into a 
 *	QList of ComponentInstances (really just a struct with a couple of strings).
 *	If something goes wrong, or no director instances are found, an empty
 *	list is returned.
 */  
QList<RemoteViewer::ComponentInstance> RemoteViewer::getDirectorList()
{
	Q_ASSERT(serverChannel_->incomingResponsesWaiting() == 0);
	QList<ComponentInstance> directors;

	if(!serverChannel_->assureConnection())
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
			ComponentInstance director;

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
				else if(xmlReader.name() == "Id" && xmlReader.isStartElement())
				{
					director.id = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Status" && xmlReader.isStartElement())
				{
					director.status = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Session-ID" && xmlReader.isStartElement())
				{
					director.sessionID = xmlReader.readElementText();
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
 *	QList of ComponentInstances (really just a struct with a few strings).
 *	If something goes wrong, or no proxy instances are found, an empty
 *	list is returned.
 */  
QList<RemoteViewer::ComponentInstance> RemoteViewer::getProxyList()
{
	Q_ASSERT(serverChannel_->incomingResponsesWaiting() == 0);
	QList<ComponentInstance> proxies;

	if(!serverChannel_->assureConnection())
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
			ComponentInstance proxy;

			while(!xmlReader.atEnd())
			{
				xmlReader.readNext();

				if(xmlReader.name() == "Name" && xmlReader.isStartElement())
				{
					proxy.name = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Address" && xmlReader.isStartElement())
				{
					proxy.address = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Id" && xmlReader.isStartElement())
				{
					proxy.id = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Status" && xmlReader.isStartElement())
				{
					proxy.status = xmlReader.readElementText();
				}
				else if(xmlReader.name() == "Proxy" && xmlReader.isEndElement())
				{
					proxies.append(proxy);
					break;
				}
			}
		}
	}
	return proxies;
}

/*! \brief checks to see if the current proxy or director has timed out, or if we have lost contact with the server
 *
 *	When we are in a session, this slot is called periodically, to confirm that the
 *	proxy and director being used haven't disapearred from the network.  Since this
 *	gives us access to the director status, it makes sense to use this time to see
 *	if the director sttae has been modified by someone else (for example, if the 
 *	director has been paused by another Workstation, we should change our state to paused)
 */
void RemoteViewer::checkForTimeouts()
{
	//check that the command channels are still connected
	if(!serverChannel_->assureConnection() ||
	   !engineSlaveChannel_->assureConnection() ||
	   !behavioralDataChannel_->assureConnection())
	{
		//connectAction_->setChecked(false);//For a checkable action, the checked property is toggled during the trigger call.
		if(connectAction_->isChecked())
		{
			connectAction_->trigger();
			QMessageBox::critical(0,tr("Server Connection Lost"),
				tr("Workstation is no longer able to connect to the server.  " 
				"The session has been ended."));
		}
		return;
	}

	//Check that the director is still alive and possibly modify our state to match it's state
	//(In case another workstation changed its state.
	ComponentStatus remoteStatus = directorStatus(directorListBox_->itemData(directorListBox_->currentIndex()).toString());

	if((remoteStatus < Stopped) && (localStatus_ != Idle))
	{
		//This means that the session has ended, so we must  disconnect
		connectAction_->setChecked(false);
		changeConnectionState(false);
	}
	else if(remoteStatus == Stopped && localStatus_ != Stopped)
	{
		engine_->stop();
		localStatus_ = Stopped;
	}
	else if(remoteStatus == Paused && localStatus_ != Paused)
	{
			engine_->pause();
			localStatus_ = Paused;
	}
	else if(remoteStatus == Running)
	{
		if(localStatus_ == Paused)
		{
			localStatus_ = Running;
			engine_->resume();
		}
		else if(localStatus_ == Stopped)
		{
			//If director went from stopped to running, then
			//a new task was started.  The easiest way for us to handle
			//this will be to disjoin and then rejoin the session,
			//but that only works if we were the observers
			disjoinSession();
			joinSession();
		}
	}
	else if(remoteStatus == NotFound)
	{
		//connectAction_->setChecked(false); //For a checkable action, the checked property is toggled during the trigger call.
		connectAction_->trigger();
		QMessageBox::critical(0,tr("Director Lost"),
			tr("The Director instance being used is no longer on the network."));
		return;
	}

	//Check the proxy
	//If there's no Proxy in this experiment, don't worry about it.
	if(proxyListBox_->currentIndex() == 0)
		return;
	//Check that the proxy is still alive.
	remoteStatus = proxyStatus(proxyListBox_->itemData(proxyListBox_->currentIndex()).toString());
	if(remoteStatus == NotFound)
	{
		//connectAction_->setChecked(false);//For a checkable action, the checked property is toggled during the trigger call.
		connectAction_->trigger();
		QMessageBox::critical(0,tr("Proxy Lost"),
			tr("The Proxy instance being used is no longer on the network."));
		return;
	}

}


/*! \brief Starts a new session
 *
 *	This starts a new session on the server.  The act of starting a session basically 
 *	ties together the various components involved (Director, and proxy) and 
 *	starts a new session database.  Note that the Workstation instance isn't really 
 *	tied to the session.  This is intentional, and makes it easy to remotely view and
 *	control running sessions.
 *
 *	Note that starting the session is really just a remote command and has no effect 
 *	on the local workstation.  If we want to actually watch the session, we'll then need 
 *	to join the session (joinSession()).
 *
 *	The order of function calls that would occur for a single user running an experiment
 *	will look like this:
 *		startSession()
 *		joinSession()
 *		disjoinSession()
 *		endSession()
 *	If a user simply wants to observe a session that someone else started, it would
 *	look like this:
 *		joinSession()
 *		disjoinSession()
 */
bool RemoteViewer::startSession()
{
	//Check for a currently running session
	if(!sessionId_.isNull())
		return true;

	//Make a copy of the experiment
	//It would be 100 times easier to copy the experiment object directly, but it contains a
	//parameter container, which is a QObject, and can't be copied.  So instead, we generate
	//a new copy instead
	activeExperiment_ = QSharedPointer<Picto::Experiment>(Picto::Experiment::Create());

	QString expXML = experiment_->toXml();
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(expXML));
	while(xmlReader->name() != "Experiment" && !xmlReader->atEnd()) 
		xmlReader->readNext();

	if(xmlReader->atEnd())
		return false;
	if(!activeExperiment_->fromXml(xmlReader))
	{
		//delete activeExperiment_;
		return false;
	}


	QString commandStr;
	QString directorID = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
	QString proxyID = proxyListBox_->itemData(proxyListBox_->currentIndex()).toString();
	commandStr = "STARTSESSION "+directorID+"/"+proxyID+" PICTO/1.0";

	QSharedPointer<Picto::ProtocolCommand> startSessCommand(new Picto::ProtocolCommand(commandStr));

	QByteArray experimentXml = expXML.toUtf8();
	startSessCommand->setContent(experimentXml);
	startSessCommand->setFieldValue("Content-Length",QString("%1").arg(experimentXml.length()));
	startSessCommand->setFieldValue("Observer-ID",observerId_.toString());

	QSharedPointer<Picto::ProtocolResponse> loadExpResponse;

	serverChannel_->sendCommand(startSessCommand);
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
		setStatus(tr("Director instance not found with ID:") + directorID + tr(".  Experiment not loaded."));
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
			setStatus(tr("Experiment loaded on remote Director instance. Session ID: ")+ sessionId_.toString());
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

/*! \brief Ends a currently running session
 *
 *	This is the opposite of startSession.  Again note that this function has no real
 *	effect on our local workstation instance.  See the comments on startSession for
 *	a more detailed explanation.
 */
bool RemoteViewer::endSession()
{
	//Check for a currently running session
	if(sessionId_.isNull())
	{
		setStatus(tr("Not currently in a session"));
	}


	QSharedPointer<Picto::ProtocolCommand> endSessCommand(new Picto::ProtocolCommand("ENDSESSION "+sessionId_+" PICTO/1.0"));
	endSessCommand->setFieldValue("Observer-ID",observerId_.toString());

	QSharedPointer<Picto::ProtocolResponse> endSessResponse;

	serverChannel_->sendCommand(endSessCommand);
	if(!serverChannel_->waitForResponse(5000))
	{
		setStatus(tr("Server did not respond to ENDSESSION command"));
		qDebug()<<(tr("Server did not respond to ENDSESSION command"));
		return false;
	}

	endSessResponse = serverChannel_->getResponse();
	if(endSessResponse->getResponseCode() == Picto::ProtocolResponseType::NotFound)
	{
		setStatus(tr("Session ID not found, session not ended"));
	}
	else if(endSessResponse->getResponseCode() == Picto::ProtocolResponseType::OK)
	{
		setStatus(tr("Session ended"));
	}
	else if(endSessResponse->getResponseCode() == Picto::ProtocolResponseType::Unauthorized)
	{
		setStatus(tr("Workstation instance not authorized to end session"));
	}
	else
	{
		setStatus(tr("Unexpected response to ENDSESSION command"));
	}
	sessionId_ = QUuid();
	serverChannel_->setSessionId(QUuid());
	//if(activeExperiment_)
		//delete activeExperiment_;
	startedSession_ = false;
	return true;

}

/*! \brief Joins a currently running session as an "observer" of the session
 *
 *	Joining a session is the act of connecting this workstation to an already 
 *	running session.  This is what allows us to see the session on the workstation
 *	screen.
 *
 *	If we just called startSession, we already know the session ID and can simply start
 *	observing that session.  If we didn't start the session, we'll use a JOINSESSION
 *	command to find the session ID attached to the director we want to watch
 */	
bool RemoteViewer::joinSession()
{
	QString directorID = directorListBox_->itemData(directorListBox_->currentIndex()).toString();

	QByteArray content;
	QSharedPointer<QXmlStreamReader> xmlReader;
	QString commandStr;

	//If we didn't start the session, we'll need to use JOINSESSION to figure out the 
	//session ID and experiment XML
	if(sessionId_.isNull())
	{

		//Send a JOINSESSION command
		commandStr = "JOINSESSION "+directorID+" PICTO/1.0";

		QSharedPointer<Picto::ProtocolCommand> joinSessCommand(new Picto::ProtocolCommand(commandStr));
		QSharedPointer<Picto::ProtocolResponse> joinSessResponse;

		serverChannel_->sendCommand(joinSessCommand);
		if(!serverChannel_->waitForResponse(5000))
		{
			setStatus(tr("Server did not respond to JOINSESSION command"));
			return false;
		}

		joinSessResponse = serverChannel_->getResponse();
		if(joinSessResponse.isNull() || joinSessResponse->getResponseCode() != Picto::ProtocolResponseType::OK)
		{
			setStatus(tr("Unexpected response to JOINSESSION. Unable to join session."));
			return false;
		}
	
		content = joinSessResponse->getDecodedContent();
		xmlReader = QSharedPointer<QXmlStreamReader>(new QXmlStreamReader(content));

		//Extract the sessionID
		while(!xmlReader->atEnd() && !(xmlReader->isStartElement() && xmlReader->name() == "SessionID"))
			xmlReader->readNext();

		if(!xmlReader->atEnd())
		{
			sessionId_ = QUuid(xmlReader->readElementText());
		}
		else
		{
			setStatus(tr("SessionID not found in response from server."));
			return false;
		}

		//Extract the experiment xml
		activeExperiment_ = QSharedPointer<Picto::Experiment>(Picto::Experiment::Create());
		while(!xmlReader->atEnd() && !(xmlReader->isStartElement() && xmlReader->name() == "Experiment"))
			xmlReader->readNext();

		if(!xmlReader->atEnd())
		{
			if(!activeExperiment_->fromXml(xmlReader))
			{
				setStatus(tr("Unable to deserialize Experiment returned by JOINSESSION"));
				return false;
			}
			generateTaskList();
		}
		else
		{
			setStatus(tr("No Experiment returned by JOINSESSION"));
			return false;
		}

	}

	serverChannel_->setSessionId(sessionId_);
	engineSlaveChannel_->setSessionId(sessionId_);
	behavioralDataChannel_->setSessionId(sessionId_);

	bool tryAgain;
	double lastTransitionTime;
	QString taskName;
	QStringList currentStateMachinePath;
	QString currentState;
	do
	{
		tryAgain = false;
		//Figure out where we are in the state machine, and what the current time is
		commandStr = "GETDATA StateDataUnit:-1.0 PICTO/1.0";

		QSharedPointer<Picto::ProtocolCommand> getDataCommand(new Picto::ProtocolCommand(commandStr));
		QSharedPointer<Picto::ProtocolResponse> getDataResponse;

		serverChannel_->sendCommand(getDataCommand);
		if(!serverChannel_->waitForResponse(5000))
		{
			setStatus(tr("Server did not respond to GETDATA command"));
			return false;
		}

		getDataResponse = serverChannel_->getResponse();

		if(getDataResponse.isNull() || getDataResponse->getResponseCode() != Picto::ProtocolResponseType::OK)
		{
			setStatus("Unexpected response to GETDATA command");
			return false;
		}

		content = getDataResponse->getDecodedContent();
		xmlReader = QSharedPointer<QXmlStreamReader>(new QXmlStreamReader(content));
		
		//Grab the data
		while(!xmlReader->atEnd() && xmlReader->readNext() && xmlReader->name() != "Data");

		//This means that the response didn't contain a <DATA> tag
		if(xmlReader->atEnd())
		{
			setStatus("GETDATA response didn't contain <Data> tag");
			return false;
		}

		while(!xmlReader->atEnd() && xmlReader->readNext() && xmlReader->name() != "StateDataUnit");

		if(!xmlReader->atEnd())
		{
			//If we're here, there is state data to read, meaning that an experiment is in progress

			Picto::StateDataUnit data;
			data.fromXml(xmlReader);

			lastTransitionTime = data.getTime();
			currentStateMachinePath = data.getMachinePath().split("::");
			currentState = data.getDestination();
			taskName = currentStateMachinePath.first();

			//Update the task list box
			int taskIdx = taskListBox_->findText(taskName);
			Q_ASSERT_X(taskIdx >= 0,"RemoteViewer::joinSession", "Task name not found in out list of experiments.");
			taskListBox_->setCurrentIndex(taskIdx);
		}
		else
		{
			//If we're here, then there is no state data, which means that a session was just 
			//started.
			
			lastTransitionTime = 0.0;
			currentStateMachinePath = QStringList();
			currentState = "";
			taskName="";
			ComponentStatus remoteStatus = directorStatus(directorID);
			// In some cases we happen to request data exactly when the director
			// has started running but hasn't yet reported its state, or has stopped
			// and has no currentState but hasn't yet informed us of the stop.  
			// This should catch these cases.
			if((remoteStatus == Running) || (remoteStatus == Paused))
				tryAgain = true;
		}
		
	} while(tryAgain);

	///////TESTING
	//We'll need to actually determine the task name directly....
	//modifiedTaskName = taskListBox_->currentText();
	//modifiedTaskName = modifiedTaskName.simplified().remove(' ');
	
	//Set ouselves up so we are synched with the remote director
	Picto::StateMachine::resetSlaveElements(lastTransitionTime);
	QSharedPointer<Picto::NetworkSignalChannel> sigChan;
	sigChan = engine_->getSignalChannel("PositionChannel").staticCast<Picto::NetworkSignalChannel>();
	sigChan->setLastTimeDataCollected(lastTransitionTime);

	//Start the timeout timer
	if(!timeoutTimer_)
	{
		timeoutTimer_ = new QTimer(this);
		timeoutTimer_->setInterval(500);
		connect(timeoutTimer_, SIGNAL(timeout()), this, SLOT(checkForTimeouts()));
	}
	timeoutTimer_->start();

	setStatus(tr("Existing session joined. Session ID: ")+ sessionId_.toString());

	//Send an isauthorized TASK command to figure out if we're authorized to send commands
	//on this session.
	if(sendTaskCommand("isauthorized"))
	{
		enableTaskCommands_ = true;
		startedSession_ = true;
	}
	else
		enableTaskCommands_ = false;



	//Finally figure out what the status of the remote director is (stopped, running, or paused)
	//and get our director running in that state.
	ComponentStatus remoteStatus = directorStatus(directorID);
	if(remoteStatus < Stopped)
	{
		setStatus("Attempted to join a session with a director that isn't in an active session");
		return false;
	}
	else if(remoteStatus == Stopped)
	{
		localStatus_ = Stopped;
		updateActions();
		return true;
	}
	else if(remoteStatus == Running || remoteStatus == Paused)
	{
		////////////TESTING
		qDebug()<<"JumpToState "<<currentStateMachinePath<<currentState;

		if(!activeExperiment_->jumpToState(currentStateMachinePath, currentState))
		{
			////////TESTING
			Q_ASSERT(false);

			setStatus("Failed to jump into the state machine correctly");
			return false;
		}
		
		if(remoteStatus == Paused)
		{
			localStatus_ = Paused;
			engine_->pause();
		}
		else
		{
			localStatus_ = Running;
		}

		updateActions();

		//WARNING:  Nothing after this line will be processed until the task is finished running
		activeExperiment_->runTask(taskName.simplified().remove(' '), engine_);
		localStatus_ = Stopped;
		updateActions();
		renderingTarget_->showSplash();
		return true;
	}

	return false;

}



/*! \brief Disjoins a currently running session
 *
 *	This is the opposite of the join function.  This detaches us from a running session 
 *	(but it doesn't actually end the session0.
 */
bool RemoteViewer::disjoinSession()
{
	//Check for a currently running session
	//if(sessionId_.isNull())
	//{
	//	setStatus(tr("Not currently in a session"));
	//	return false;
	//}

	timeoutTimer_->stop();

	//If we're running, we should stop the local engine
	if(localStatus_ > Stopped)
		engine_->stop();

	localStatus_ = Stopped;
	engineSlaveChannel_->setSessionId(QUuid());
	behavioralDataChannel_->setSessionId(QUuid());

	////If we started the session, there's no reason to throw away the session ID
	////until we end it.
	//if(!startedSession_)
	//{
		sessionId_ = QUuid();
		//delete activeExperiment_;
	//}

	setStatus("Disjoined session");

	return true;
}

//! Sends a task command with the given target.  Returns true if execution was successful
bool RemoteViewer::sendTaskCommand(QString target)
{
	QSharedPointer<Picto::ProtocolCommand> cmd(new Picto::ProtocolCommand("TASK "+target+" PICTO/1.0"));
	cmd->setFieldValue("Observer-ID",observerId_.toString());
	
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
	else if(cmdResponse->getResponseCode() == Picto::ProtocolResponseType::Unauthorized)
	{
		setStatus("TASK "+target+tr(" command failed: Workstation instance not authorized"));
		return false;
	}
	else
	{
		Q_ASSERT(false);
		return false;
	}
}

void RemoteViewer::taskListIndexChanged(int)
{
	if(!experiment_)
		return;
	QSharedPointer<Task> task = experiment_->getTaskByName(taskListBox_->currentText());
	if(!task)
		return;
	qobject_cast<PropertyFrame*>(propertyFrame_)->setTopLevelDataStore(task.staticCast<DataStore>());
}