#include <QApplication>
#include "ComponentInterface.h"
#include "../common/network/ServerDiscoverer.h"
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>
ComponentInterface::ComponentInterface()
{
	deviceOpened_ = false;
	sessionStarted_ = false;
	expStarted_ = false;
	serverWasConnected_ = false;
	setStatus(idle);
}
int ComponentInterface::activate()
{
	active_ = true;
	deviceOpened_ = true;
	QString dbName = "Picto" + componentType() + "Interface";
	dbName = dbName.toLower();
	configDb_ = QSqlDatabase::addDatabase("QSQLITE",dbName);
	configDb_.setDatabaseName(QCoreApplication::applicationDirPath() + "/" + dbName + ".config");
	configDb_.open();

	QUuid componentID;
	QSqlQuery query(configDb_);
	if(!configDb_.tables().contains("componentinfo"))
	{
		query.exec("CREATE TABLE componentinfo (key TEXT, value TEXT)");
		componentID = QUuid::createUuid();
		query.prepare("INSERT INTO componentinfo (key,value) VALUES ('id',:id)");
		query.bindValue(":id",QString(componentID));
		query.exec();
	}
	query.exec("SELECT value FROM componentinfo WHERE key='id'");
	Q_ASSERT(query.next());
	componentID = QUuid(query.value(0).toString());

	openDevice();

	// Setup networking channel pointers

	///////////////////////////////////////
	// Run event loop
	///////////////////////////////////////

	//Once we have the command channel set up, we'll sit in a tight loop sending
	//COMPONENTUPDATE commands at 1Hz and processing the responses forever.
	QSharedPointer<Picto::ProtocolResponse> updateResponse;
	QString updateCommandStr = "COMPONENTUPDATE "+name()+":idle PICTO/1.0";
	QSharedPointer<Picto::ProtocolCommand> updateCommand(new Picto::ProtocolCommand(updateCommandStr));
	while(active_ && continueRunning())
	{
		//Check on network connection
		if(serverUpdateChannel_.isNull() || !serverUpdateChannel_->isConnected())
		{
			ServerConnectivityUpdate(false);
			setStatus(idle);
			serverUpdateChannel_ = connectToServer(componentID, componentType());
			continue;
		}

		//Check on network connection
		if(dataCommandChannel_.isNull() || !dataCommandChannel_->isConnected())
		{
			ServerConnectivityUpdate(false);
			setStatus(idle);
			dataCommandChannel_ = connectToServer(componentID, componentType());
			continue;
		}
		ServerConnectivityUpdate(true);

		if(serverUpdateChannel_->incomingResponsesWaiting())
		{
			int pendingResponses = serverUpdateChannel_->pendingResponses();
			
			updateResponse = serverUpdateChannel_->getResponse();
			QString debugMsg = "Unexpected response: ";
			debugMsg += updateResponse->generateHeadersString();
			debugMsg += updateResponse->getContent();
			Q_ASSERT_X(false, "Component loop", QString::number(pendingResponses).toAscii());
			Q_ASSERT_X(false, "Component loop", debugMsg.toAscii());
		}
		//If I'm idle, then I have no sessionID
		if(getStatusString() == "idle")
		{
			serverUpdateChannel_->setSessionId(QUuid());
			dataCommandChannel_->setSessionId(QUuid());
		}

		updateCommand->setTarget(name()+":"+getStatusString());
		serverUpdateChannel_->sendCommand(updateCommand);

		//If we don't get a response, it probably means that we lost our
		//network connection, so start over
		if(!serverUpdateChannel_->waitForResponse(5000))
			continue;

		updateResponse = serverUpdateChannel_->getResponse();
		if(updateResponse.isNull() || updateResponse->getResponseType() != "OK")
		{
			continue;
		}

		//serverActivity();
		QString statusDirective = updateResponse->getDecodedContent();
		if(statusDirective.startsWith("OK"))
		{
			//We don't need to do anything if the status is "OK"
		}
		else if(statusDirective.startsWith("NEWSESSION"))
		{
			QUuid sessionID = QUuid(statusDirective.mid(statusDirective.indexOf(' ')+1));
			serverUpdateChannel_->setSessionId(sessionID);
			dataCommandChannel_->setSessionId(sessionID);
			setStatus(stopped);
			sessionStarted_ = true;
			startSession(sessionID);
		}
		else if(statusDirective.startsWith("LOADEXP"))
		{
			//Extract the XML from the response
			int xmlIdx = statusDirective.indexOf('\n');
			statusDirective.remove(0,xmlIdx+1);
			loadExp(statusDirective);
		}
		else if(statusDirective.startsWith("START"))
		{
			QString taskName = statusDirective.mid(statusDirective.indexOf(' ')+1);
			//Before we actually start running the task, we need to send a COMPONENTUPDATE
			//command to let the server know that we have changed status to running
			//This also lets Workstation know that we received the command that they
			//initiated.  Note that if there is a directive in the response, it will be
			//ignored.  I don't think this is a problem, since there would be no reason
			//to send a second directive immediately after the first.
			QString runningUpdateCommandStr = "COMPONENTUPDATE "+name()+":running PICTO/1.0";
			QSharedPointer<Picto::ProtocolCommand> runningUpdateCommand(new Picto::ProtocolCommand(runningUpdateCommandStr));
			QSharedPointer<Picto::ProtocolResponse> runningUpdateResponse;

			serverUpdateChannel_->sendCommand(runningUpdateCommand);

			if(!serverUpdateChannel_->waitForResponse(5000))
				continue;
			runningUpdateResponse = serverUpdateChannel_->getResponse();
			if(runningUpdateResponse.isNull() || runningUpdateResponse->getResponseType() != "OK")
				continue;

			expStarted_ = true;
			startExp(taskName);
			expStarted_ = false;
		}
		else if(statusDirective.startsWith("ENDSESSION"))
		{
			endSession();
			sessionStarted_ = false;
			serverUpdateChannel_->setSessionId(QUuid());
			dataCommandChannel_->setSessionId(QUuid());
			setStatus(idle);
		}
		else if(statusDirective.startsWith("REWARD"))
		{
			int channel = statusDirective.split(" ").value(1).toInt();
			reward(channel);
		}
		else if(statusDirective.startsWith("ERROR"))
		{
			reportErrorDirective();
		}
		else
		{
			reportUnsupportedDirective(statusDirective);
		}
	
		//Pause for 20 ms 
		QEventLoop pauseLoop;
		QTimer pauseTimer;
		pauseTimer.setSingleShot(true);
		pauseTimer.setInterval(20);
		QObject::connect(&pauseTimer, SIGNAL(timeout()), &pauseLoop, SLOT(quit()));
		pauseTimer.start();
		pauseLoop.exec();
	}
	ServerConnectivityUpdate(false);
	return 0;
}

QSharedPointer<Picto::CommandChannel> ComponentInterface::connectToServer(QUuid componentID, QString componentType)
{
	QSharedPointer<Picto::CommandChannel> serverChannel(new Picto::CommandChannel(componentID,componentType));
	QSharedPointer<Picto::CommandChannel> nullChannel;

	//Find a server and open a command channel to it
	Picto::ServerDiscoverer serverDiscoverer;

	serverDiscoverer.discover();

	if(!serverDiscoverer.waitForDiscovered(10000))
		return nullChannel;

	serverChannel->connectToServer(serverDiscoverer.getAddress(), 
								 serverDiscoverer.getPort());

	Q_ASSERT(serverChannel->getChannelStatus() == Picto::CommandChannel::connected);

	return serverChannel;
}
QString ComponentInterface::getStatusString()
{
	switch(status_)
	{
	case idle:
		return "idle";
	case stopped:
		return "stopped";
	case paused:
		return "paused";
	case running:
		return "running";
	}
	return "";
}
int ComponentInterface::deActivate()
{
	active_ = false;
	if(expStarted_)
		stopExp();
	expStarted_ = false;
	if(sessionStarted_)
		endSession();
	sessionStarted_ = false;
	if(deviceOpened_)
		closeDevice();
	deviceOpened_ = false;
	return 0;
}

void ComponentInterface::ServerConnectivityUpdate(bool connected)
{
	if(serverWasConnected_ != connected)
		reportServerConnectivity(connected);
	serverWasConnected_ = connected;
}