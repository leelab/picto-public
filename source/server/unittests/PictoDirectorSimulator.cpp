#include "PictoDirectorSimulator.h"
#include "PictoServerSimulator.h"
#include "../../common/network/serverdiscoverer.h"
using namespace PictoSim;
using namespace Director;

PictoDirectorSimulator::PictoDirectorSimulator(PictoSystemState* systemState, QString deviceName) : PictoDeviceSimulator(systemState, deviceName)
{
}

QString PictoDirectorSimulator::GetDeviceTypeName()
{
	return "DIRECTOR_SIMULATOR";
}

void PictoDirectorSimulator::Init()
{
	myIPAddress_ = QString("0.0.0.0");
	session_ = "{00000000-0000-0000-0000-000000000000}";
	// Tcp socket must be created in the thread where it will be used
	tcpSocket_ = QSharedPointer<QTcpSocket>(new QTcpSocket());
	timerRunning_ = false;
}

void PictoDirectorSimulator::Deinit()
{
	// tcp socket must be deleted in the thread where it was created.
	tcpSocket_ = QSharedPointer<QTcpSocket>();
}

void PictoDirectorSimulator::Act(QSharedPointer<SimActionDesc> actionDesc)
{
	QString message;
	switch(actionDesc->type_)
	{
	case DISCOVERSERVER:
		{
			systemState_->AssureDeviceExistance(systemState_->GetServerName(),SERVER);
			Picto::ServerDiscoverer serverDiscoverer;
			serverDiscoverer.discover();
			if(!serverDiscoverer.waitForDiscovered(10000))
			{
				QFAIL((GetDeviceTypeName() + " did not recieve a response to its DISCOVER call from the Server").toAscii() );
				return;
			}
			ConnectToHost(tcpSocket_,serverDiscoverer.getAddress(),serverDiscoverer.getPort());
		}
		break;
	case COMPONENTUPDATE:	// This runs until a task is started
		{
			QString updateTemplate = "COMPONENTUPDATE " + GetDeviceName() + ":%1 PICTO/1.0\r\nSession-ID:%2\r\n\r\n";
			QString runningStatus = "running";
			QString pausedStatus = "paused";
			QString stoppedStatus = "stopped";
			QString idleStatus = "idle";
			QString updateMsg = updateTemplate.arg(idleStatus).arg(session_);
			int sleepTime = 1000;
			bool running = false;
			// Loop until we get broken out by the test bench.  For each loop, send an update message, check response,
			// check if the message needs to be updated, if so change it, go to sleep and start again.
			do{
				SendMessage(updateMsg, tcpSocket_);
				ReadIncomingMessage("PICTO/1.0 200",message,tcpSocket_,10000);
				sleepTime = 1000;	// 1 second between messages when there's nothing new to report
				QString directive = message.split("\r\n\r\n")[1];
				if((directive != "OK") && (directive != ""))
					Debug() << directive << "received";
				if(directive.startsWith("NEWSESSION"))
				{
					// If server said NEWSESSION, store the session and reply that we're stopped.
					sleepTime = 0;
					QStringList bufferList = message.split("NEWSESSION ");
					if(bufferList.size() > 1)
					{
						session_ = bufferList[1].split("}")[0] + "}";
					}
					updateMsg = updateTemplate.arg(stoppedStatus).arg(session_);
					running = false;
				}
				else if(directive.startsWith("ENDSESSION"))
				{
					// If server said ENDSESSION, get rid of session number and reply that we're idle.
					sleepTime = 0;
					session_ = "{00000000-0000-0000-0000-000000000000}";
					updateMsg = updateTemplate.arg(idleStatus).arg(session_);
					running = false;
				}
				else if(directive.startsWith("LOADEXP"))
				{
					//Don't worry about LOADEXP (we aren't running anything anyway)
					running = false;
				}
				else if(directive.startsWith("START"))
				{
					//When the server says to START, reply that we're running and set running to true
					//so we can start sending messages.
					sleepTime = 0;
					updateMsg = updateTemplate.arg(runningStatus).arg(session_);
					running = true;
				}
				else if(directive.startsWith("STOP"))
				{
					//If server said STOP, reply that we're stopped and set running to false so we will
					//stop sending messages.
					sleepTime = 0;
					updateMsg = updateTemplate.arg(stoppedStatus).arg(session_);
					running = false;
				}
				else if(directive.startsWith("PAUSE"))
				{
					//If server says to PAUSE, replay that we're paused and set running to false so we will
					//stop sending messages.
					sleepTime = 0;
					updateMsg = updateTemplate.arg(pausedStatus).arg(session_);
					running = false;
				}
				else if (running)
				{
					//If we're running, use SimulateNextMessage to wait however long is necessary and then 
					//send whatever message is next in the actionDesc's list.
					updateMsg = SimulateNextMsg(actionDesc);
					if(updateMsg == "")
					{
						updateMsg = updateTemplate.arg(idleStatus).arg(session_);
						running = false;
					}
					sleepTime = 0;
				}
				else if(directive.startsWith("OK"))
				{
					//There's no directive from the server, so just keep doing what you're doing.
					sleepTime = 1000;
				}
				
				// Tell simulated server that it knows about me
				QSharedPointer<PictoServerSimulator> server = systemState_->GetDevice(systemState_->GetServerName()).staticCast<PictoServerSimulator>();
				server->knownDirectors_.insert(GetDeviceName(),systemState_->GetDevice(GetDeviceName()));
			} while(!InterruptableSleep(sleepTime));
		}
		break;
	case PUTDATA:
		SendMessage((QString("PUTDATA ") + GetDeviceName() + " PICTO/1.0\r\nSession-ID:%1\r\nContent-Length:69\r\n\r\n<BehavioralDataUnitPackage><Data time=2.4 x=50 y=30/></BehavioralDataUnitPackage>"), tcpSocket_);
		ReadIncomingMessage("PICTO/1.0 200 OK",message,tcpSocket_,10000);
		break;
	case TRIAL:
		QSharedPointer<TrialDesc> desc = actionDesc.staticCast<TrialDesc>();
		SendMessage(QString("TRIAL /start PICTO/1.0\r\nSession-ID:%1\r\nContent-Length:79\r\n\r\n<Trial><Time>1.223</Time><EventCode>1</EventCode><TrialNum>1</TrialNum></Trial>").arg(session_), tcpSocket_);
		ReadIncomingMessage("PICTO/1.0 200 OK",message,tcpSocket_,10000);
		InterruptableSleep(desc->msInterTrialDelay_);
		break;
	}
}

/*! \brief Sleeps until the next message on actionDesc's timestamp time arrives, then returns a suitable TRIAL or PUTDATA message.
 *	If an empty string is returned, there are no more actions on actionDesc's list.
 */
QString PictoDirectorSimulator::SimulateNextMsg(QSharedPointer<SimActionDesc> actionDesc)
{
	QString result = "";
	QSharedPointer<DirectorUpdateDesc> action = actionDesc.staticCast<DirectorUpdateDesc>();
	if(!action->msgs_.size())
		return result;
	QSharedPointer<SimMsgDesc> msg = action->msgs_.front();
	action->msgs_.pop_front();
	if(!timerRunning_)
	{
		timerRunning_ = true;
		timer_.start();
	}
	int sleepTime = msg->timestamp_ - timer_.elapsed();
	if(sleepTime < 0)sleepTime = 0;
	if(InterruptableSleep(sleepTime))
		return result;
	QString trialTemplate = "TRIAL /start PICTO/1.0\r\nSession-ID:%1\r\nContent-Length:%2\r\n\r\n<Trial><Time>%3</Time><EventCode>%4</EventCode><TrialNum>%5</TrialNum></Trial>";
	QString putDataTemplate = "PUTDATA " + GetDeviceName() + " PICTO/1.0\r\nSession-ID:%1\r\nContent-Length:%2\r\n\r\n<Data><BehavioralDataUnitPackage><Data time=\"%3\" x=\"%4\" y=\"%5\"/></BehavioralDataUnitPackage></Data>";
	switch(msg->type_)
	{
	case DirectorUpdateDesc::TRIAL:
		{
			QString timestampStr = QString::number(msg->timestamp_);
			QString eventCode = QString::number(msg.staticCast<DirectorUpdateDesc::TrialMsgDesc>()->eventID_);
			QString trialNum = QString::number(msg.staticCast<DirectorUpdateDesc::TrialMsgDesc>()->trialID_);
			result = trialTemplate.arg(session_).arg(72+timestampStr.length()+eventCode.length()+trialNum.length()).arg(timestampStr).arg(eventCode).arg(trialNum);	
			Debug() << "Sending TRIAL Message";
		}
		break;
	case DirectorUpdateDesc::DATA:
		{
			QString timestampStr = QString::number(msg->timestamp_);
			QString x = QString::number(msg.staticCast<DirectorUpdateDesc::DataMsgDesc>()->x_);
			QString y = QString::number(msg.staticCast<DirectorUpdateDesc::DataMsgDesc>()->y_);
			result = putDataTemplate.arg(session_).arg(81+timestampStr.length()+x.length()+y.length()).arg(timestampStr).arg(x).arg(y);	
			Debug() << "Sending DATA Message";
		}
		break;
	}
	return result;
}