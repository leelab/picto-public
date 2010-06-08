#include "remoteviewer.h"

#include "../common/globals.h"
#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"



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
	serverChannel_(0)
{
	setupServerChannel();
	setupEngine();
	setupUi();

	updateTimer_ = new QTimer(this);
	updateTimer_->setInterval(5000);
	connect(updateTimer_, SIGNAL(timeout()), this, SLOT(updateStatus()));
	connect(updateTimer_, SIGNAL(timeout()), this, SLOT(updateDirectorList()));
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
	////If we're stopped, load the current experiment.  If we are paused,
	////then we shouldn't load the experiment.
	//if(status_ == Stopped)
	//{
	//	if(!experiment_ || !engine_->loadExperiment(experiment_))
	//	{
	//		QMessageBox msg;
	//		msg.setText("Failed to load current experiment.");
	//		msg.exec();

	//	}
	//}

	generateTaskList();
	
	updateActions();
	updateDirectorList();
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

	//Set up the rendering target
	/*QSharedPointer<Picto::PCMAuralTarget> pcmAuralTarget(new Picto::PCMAuralTarget());
	pixmapVisualTarget_ = QSharedPointer<Picto::PixmapVisualTarget>(new Picto::PixmapVisualTarget(true,800,600));
	renderingTarget_ = QSharedPointer<Picto::RenderingTarget>(new Picto::RenderingTarget(pixmapVisualTarget_, pcmAuralTarget));
	engine_->addRenderingTarget(renderingTarget_);

	//Set up the visual target host
	//This exists because QSharedPointer<QWidget> results in multiple delete call, which 
	//gives us memory exceptions.
	visualTargetHost_ = new Picto::VisualTargetHost;
	visualTargetHost_->setVisualTarget(pixmapVisualTarget_);

	//set up signal channel
	//We'll need to set up a network signal channel here...

	//Set up event code generator
	QSharedPointer<Picto::EventCodeGenerator> nullGenerator;
	nullGenerator = QSharedPointer<Picto::EventCodeGenerator>(new Picto::NullEventCodeGenerator());
	engine_->setEventCodeGenerator(nullGenerator);

	//set up reward controller
	QSharedPointer<Picto::RewardController> rewardController;
	rewardController = QSharedPointer<Picto::RewardController>(new Picto::AudioRewardController());
	engine_->setRewardController(rewardController);*/
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

	//TaskList combo box
	taskListBox_ = new QComboBox;
	taskListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	generateTaskList();

	//The toolbar
	toolBar_ = new QToolBar;

	toolBar_->addAction(playAction_);
	toolBar_->addAction(pauseAction_);
	toolBar_->addAction(stopAction_);
	toolBar_->addSeparator();
	toolBar_->addWidget(new QLabel("Task: ", this));
	toolBar_->addWidget(taskListBox_);
	toolBar_->addSeparator();

	//----- Connection Actions -----
	connectAction_ = new QAction(tr("&Connect"), this);
	connectAction_->setCheckable(true);
	connectAction_->setIcon(QIcon(":/icons/disconnected.png"));
	connect(connectAction_, SIGNAL(triggered(bool)), this, SLOT(changeConnectionState()));


	directorListBox_ = new QComboBox;
	directorListBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	updateDirectorList();


	toolBar_->addAction(connectAction_);
	//toolBar_->addAction(disconnectAction_);
	toolBar_->addWidget(new QLabel(Picto::Names->directorAppName + ": ", this));
	toolBar_->addWidget(directorListBox_);
	
	//------ Main layout -----------
	QHBoxLayout *toolbarLayout = new QHBoxLayout;
	toolbarLayout->addWidget(toolBar_);
	toolbarLayout->addStretch();

	statusBar_ = new QLabel("Testing");

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(toolbarLayout);
	//mainLayout->addWidget(visualTargetHost_);
	mainLayout->addStretch(1);
	mainLayout->addWidget(statusBar_);
	setLayout(mainLayout);

	updateActions();

}

void RemoteViewer::setupServerChannel()
{
	serverChannel_ = new Picto::CommandChannel(this);
	serverChannel_->pollingMode(true);
	serverDiscoverer_ = new Picto::ServerDiscoverer(this);

	connect(serverDiscoverer_, SIGNAL(foundServer(QHostAddress, quint16)), serverChannel_, SLOT(connectToServer(QHostAddress, quint16)));
	connect(serverDiscoverer_, SIGNAL(discoverFailed()), serverDiscoverer_, SLOT(discover()));
	connect(serverChannel_, SIGNAL(droppedConnection()), serverDiscoverer_, SLOT(discover()));

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
			status_ = Running;
	}
	else if(status_ == Paused)
	{
		if(sendTaskCommand("resume"))
			status_ = Running;
	}
	updateActions();
}

void RemoteViewer::pause()
{
	if(sendTaskCommand("pause"))
	{
		status_ = Paused;
	}
	updateActions();
}

void RemoteViewer::stop()
{
	if(sendTaskCommand("stop"))
	{
		status_ = Stopped;
	}
	updateActions();
}

//! \brief Called whenever the connected button changes state
void RemoteViewer::changeConnectionState()
{
	if(connectAction_->isChecked())
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
	//------------------------------------------
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
	//------------------------------------------
	bool disableStop = false;
	disableStop |= !(status_ == Running || status_ == Paused);
	disableStop |= !connectAction_->isChecked();

	if(disableStop)
		stopAction_->setEnabled(false);
	else
		stopAction_->setEnabled(true);


	//Pause action
	//------------------------------------------
	bool disablePause = false;
	disablePause |= (status_ != Running);
	disablePause |= !connectAction_->isChecked();

	if(disablePause)
		pauseAction_->setEnabled(false);
	else
		pauseAction_->setEnabled(true);

	//Director Combo Box
	//------------------------------------------
	bool disableDirectorList = false;
	disableDirectorList |= connectAction_->isChecked();

	if(disableDirectorList)
		directorListBox_->setEnabled(false);
	else
		directorListBox_->setEnabled(true);

	//Connect Action
	//------------------------------------------
	bool disableConnect = false;
	disableConnect |= (directorListBox_->count() == 0);

	if(disableConnect)
		connectAction_->setEnabled(false);
	else
		connectAction_->setEnabled(true);

}

/*! \updates the combo box containing Director instances
 *
 *	The list of director instances gets updated periodically (by a running timer).
 *	This function asks the server for a current list of Director instances, and 
 *	then adds new ones to the combo box and removes old ones, as needed
 */
void RemoteViewer::updateDirectorList()
{
	if(status_ != Stopped)
		return;

	QList<DirectorInstance> directors;
	directors = getDirectorList();
	
	//Clear out the combo box, then add all directors that are available
	directorListBox_->clear();

	for(int i=0; i<directors.length(); i++)
	{
		DirectorInstance d = directors[i];
		directorListBox_->addItem(d.name, d.address);
	}

	directorListBox_->setEnabled(directorListBox_->count() > 0);

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

//Starts a new session
bool RemoteViewer::startSession()
{
	//Check for a currently running session
	if(!sessionId_.isNull())
		return true;

	QString commandStr;
	QString directorAddr = directorListBox_->itemData(directorListBox_->currentIndex()).toString();
	commandStr = "STARTSESSION "+directorAddr+" PICTO/1.0";

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
		qDebug()<<(tr("Director instance not found at:") + directorAddr +tr(".  Experiment not loaded."));
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

//! Ends a currently running session
bool RemoteViewer::endSession()
{
	//Check for a currently running session
	if(sessionId_.isNull())
		return false;

	//If we're running, we should stop first
	if(status_ == Running || status_ == Paused)
		stop();

	QSharedPointer<Picto::ProtocolCommand> endSessCommand(
		new Picto::ProtocolCommand("ENDSESSION "+sessionId_+" PICTO/1.0"));
	QSharedPointer<Picto::ProtocolResponse> endSessResponse;

	serverChannel_->sendCommand(endSessCommand);
	if(!serverChannel_->waitForResponse(5000))
	{
		setStatus(tr("Server did not respond to ENDSESSION command"));
		qDebug()<<(tr("Server did not respond to ENDSESSION command"));
		return false;
	}

	endSessResponse = serverChannel_->getResponse();
	if(endSessResponse.isNull())
	{
		setStatus(tr("No response received from server.\nExperiment not loaded."));
		return false;
	}
	else if(endSessResponse->getResponseCode() == Picto::ProtocolResponseType::NotFound)
	{
		setStatus(tr("Session ID not found, session not ended"));
		return false;
	}
	else if(endSessResponse->getResponseCode() == Picto::ProtocolResponseType::OK)
	{
		setStatus(tr("Session ended"));
		status_ = Stopped;
		serverChannel_->setSessionId(QUuid());
		sessionId_ = QUuid();
		return true;
	}
	else
	{
		setStatus(tr("Unexpected response to ENDSESSION command"));
		return false;
	}

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