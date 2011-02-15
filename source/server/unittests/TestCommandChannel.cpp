#include "TestCommandChannel.h"
#include "ConsumerServer.h"
#include "../../common/protocol/ProtocolCommand.h"
#include "../../common/protocol/ProtocolResponse.h"

#include <QDateTime>

#include <QSignalSpy>
#include <QList>
#include <QVariant>
#include <QXmlStreamReader>
#include <QUuid>

#include <QDebug>

Q_DECLARE_METATYPE(QSharedPointer<Picto::ProtocolResponse>)
Q_DECLARE_METATYPE(QSharedPointer<Picto::ProtocolCommand>)


TestCommandChannel::TestCommandChannel(QHostAddress _serverAddress,quint16 _port) :
	serverAddress_(_serverAddress),
	port_(_port)
{
}

void TestCommandChannel::initTestCase()
{
	channel_ = new Picto::CommandChannel(QUuid::createUuid(),"TESTCOMPONENT",serverAddress_, port_, this);
	if(channel_->getChannelStatus() != Picto::CommandChannel::connected)
		QFAIL("Command channel failed to connect");
}

//This tests the command channel in a polled mode, with a single command sent
//and response recieved
//The test will be passed if the following conditions are met
//		1. Exactly one response is sitting in the incoming responses queue
//		   (true if incomingResponsesWaiting() == 1)
//		2. The response that was waiting is of type 200 (OK)
//Note that during this (and all other) polled test, the QT event loop is never given
//a chance to run (that's why we use qSleep).  This is on purpose, since if a command
//channel is used in polled mode, it is probably being used in a situation where
//there is no event loop, so we need to make sure that it can run under those
//conditions.
void TestCommandChannel::polledSingleCommand()
{
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("GET / HTTP/1.1"));
	channel_->sendCommand(command);

	//give the server and network a chance to do their stuff
	//(but don't give the event loop a chance to run)
	QTest::qSleep(500);
	
	//is there 1 and only 1 response in the queue?
	QCOMPARE(channel_->incomingResponsesWaiting(),1);

	//grab the response and confirm it is a 200 OK
	QSharedPointer<Picto::ProtocolResponse> response = channel_->getResponse();

	QCOMPARE(response->getResponseCode(), 200);
		
}

//This test sends out multiple commands with short breaks, and then at the end
//polls the command channel to see if there are any responses

//To pass, the following requirements must be met:
//		1. Number of responses = Number of commands sent
//		2. Responses are received in order (as determined by the date field)
void TestCommandChannel::polledMultipleCommands()
{
	int commandsToSend = 10;
	
	//send a command every 2 seconds (this is way slower than normal, but the date
	//field in the response only has a resolution of seconds).
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("GET / HTTP/1.1"));
	for(int i=0; i<commandsToSend; i++)
	{
		channel_->sendCommand(command);
		QTest::qSleep(2000);
	}

	//check for responses (since we already slept for 2 seconds, the responses should
	//already be in).
	QCOMPARE(channel_->incomingResponsesWaiting(), commandsToSend);

	//read each response and confirm that that it came after the previous reponse
	QDateTime lastTime = QDateTime::currentDateTime();
	lastTime.addDays(-2);
	QDateTime currentTime;

	QSharedPointer<Picto::ProtocolResponse> response;
	for(int i=0; i<commandsToSend; i++)
	{
		response = channel_->getResponse();
		currentTime = QDateTime::fromString(response->getFieldValue("Date"),"ddd, dd MMM yyyy hh:mm:ss G'M'T");
		QVERIFY(currentTime > lastTime);
		lastTime = currentTime;
	}



}

// This test confirms that the command channel can reconnect when the server 
// times out.  The test consists of sending a command in polled mode, sleeping
// long enough for the server to time out, and then sending a second command.
//	If the channel is working correctly, the second command will go through. 
//	The test will be passed if the following conditions are met:
//		1. Both commands result in responses sitting in the queue
//		2. The channel does not emit a droppedConnection signal
/*void TestCommandChannel::polledServerConnectionDropped()
{
	int serverTimeout = 10000;
	
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("GET / HTTP/1.1"));

	QSignalSpy spy(channel_, SIGNAL(droppedConnection()));

	channel_->sendCommand(command);
	QTest::qSleep(500);
	QCOMPARE(channel_->incomingResponsesWaiting(),1);

	//Sleep long enough to cause a timeout
	QTest::qSleep(serverTimeout+2000);

	channel_->sendCommand(command);
	QTest::qSleep(500);
	QCOMPARE(channel_->incomingResponsesWaiting(),1);

	QCOMPARE(spy.count(),0);
}*/

		
// This test places the command channel in the event driven mode (polled = false)
// and then sends a single command to the server.  The test uses QSignalSpy
// to watch the signals coming out of the command channel.   The test will 
// pass if the following conditions are met:
//		1. The command channel emitted exactly one incomingResponse signal
//		2. The response is of type 200 (OK)
//		3. The content of the response contains the following string:
//			"<title>Picto Server Status</title>"

/*void TestCommandChannel::eventDrivenSingleCommand()
{
	channel_->pollingMode(false);

	qRegisterMetaType< QSharedPointer<Picto::ProtocolResponse> >("QSharedPointer<Picto::ProtocolResponse>");
	QSignalSpy spy(channel_, SIGNAL(incomingResponse(QSharedPointer<Picto::ProtocolResponse>)));	
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("GET / HTTP/1.1"));
	channel_->sendCommand(command);

	//we have to wait here so that the server has a chance to respond, and the event loop
	//gets run
	QTest::qWait(50);

	QCOMPARE(spy.count(),1);  //only one response expected
	
	QList<QVariant> arguments = spy.takeFirst();
	QSharedPointer<Picto::ProtocolResponse> response = arguments[0].value<QSharedPointer<Picto::ProtocolResponse> >();
	QByteArray content = response->getDecodedContent();

	//Check that the response is "200 OK"
	QCOMPARE(response->getResponseCode(), 200);

	//make sure that the response is as expected
	QVERIFY(content.contains("<title>Picto Server Status</title>"));

}*/

// This test places the command channel in the event driven mode (polled = false)
// and then sends multiple commands to the server.  The test uses QSignalSpy
// to watch the signals coming out of the command channel.   The test will 
// pass if the following conditions are met:
//		1. The command channel emitted exactly the same number of
//		   incomingResponse signals as commands were issued
//		2. Each response is of type 200 (OK)
//		3. The content of each response contains the following string:
//			"<title>Picto Server Status</title>"
//		4. The responses arrived in the correct order (determined using
//		   the "Date:" field in the response headers
//Since the first test requires the commands to be seperated by >1 sec, the test
//is run a second time with commands issued at 5 ms intervals.  The same conditions
//are checked as above, except for condition 4.
/*void TestCommandChannel::eventDrivenMultipleCommands()
{
	int commandsToSend = 10;

	channel_->pollingMode(false);

	qRegisterMetaType< QSharedPointer<Picto::ProtocolResponse> >("QSharedPointer<Picto::ProtocolResponse>");
	QSignalSpy spy(channel_, SIGNAL(incomingResponse(QSharedPointer<Picto::ProtocolResponse>)));	
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("GET / HTTP/1.1"));

	//The 2 second wait is so we can determine the ordering (the arrival time of each
	//response is determined using the HTTP "Date" field, which only has a 1 second
	//resolution
	for(int i=0; i<commandsToSend; i++)
	{
		channel_->sendCommand(command);
		QTest::qWait(2000);
	}

	QCOMPARE(spy.count(),commandsToSend);

	//read each response and confirm that that it came after the previous reponse
	QDateTime lastTime = QDateTime::currentDateTime();
	lastTime.addDays(-2);
	QDateTime currentTime;

	while(!spy.isEmpty())
	{
		QList<QVariant> arguments = spy.takeFirst();
		QSharedPointer<Picto::ProtocolResponse> response = arguments[0].value<QSharedPointer<Picto::ProtocolResponse> >();
		QByteArray content = response->getDecodedContent();

		//Check that the response is "200 OK"
		QCOMPARE(response->getResponseCode(), 200);

		//make sure that the response is as expected
		QVERIFY(content.contains("<title>Picto Server Status</title>"));

		//check the times (did we receive the response signals in order?)
		currentTime = QDateTime::fromString(response->getFieldValue("Date"),"ddd, dd MMM yyyy hh:mm:ss G'M'T");
		QVERIFY(currentTime > lastTime);
		lastTime = currentTime;

	}

	//Send commands in quick succession (seperated by only 5 ms)
	for(int i=0; i<commandsToSend; i++)
	{
		channel_->sendCommand(command);
		QTest::qWait(5);
	}

	//wait a bit longer to give everything a chance to catch up
	QTest::qWait(500);

	QCOMPARE(spy.count(),commandsToSend);


	while(!spy.isEmpty())
	{
		QList<QVariant> arguments = spy.takeFirst();
		QSharedPointer<Picto::ProtocolResponse> response = arguments[0].value<QSharedPointer<Picto::ProtocolResponse> >();
		QByteArray content = response->getDecodedContent();

		//Check that the response is "200 OK"
		QCOMPARE(response->getResponseCode(), 200);

		//make sure that the response is as expected
		QVERIFY(content.contains("<title>Picto Server Status</title>"));
	}
}*/

// This test confirms that the command channel can reconnect when the server 
// times out.  The test consists of sending a command in eventd driven mode, sleeping
// long enough for the server to time out, and then sending a second command.
//	If the channel is working correctly, the second command will go through. 
//	The test will be passed if the following conditions are met:
//		1. Both commands result in incomingResoponse being emitted
//		2. The channel does not emit a droppedConnection signal
/*void TestCommandChannel::eventDrivenServerConnectionDropped()
{
	int serverTimeout = 10000;
	channel_->pollingMode(false);

	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("GET / HTTP/1.1"));

	qRegisterMetaType< QSharedPointer<Picto::ProtocolResponse> >("QSharedPointer<Picto::ProtocolResponse>");

	QSignalSpy incomingResponseSpy(channel_, SIGNAL(incomingResponse(QSharedPointer<Picto::ProtocolResponse>)));	
	QSignalSpy droppedConnectionSpy(channel_, SIGNAL(droppedConnection()));

	channel_->sendCommand(command);
	QTest::qWait(50);
	QCOMPARE(incomingResponseSpy.count(),1);

	//Wait long enough to cause a timeout
	//Since we're in event driven mode, we have to "wait" not "sleep"
	//otherwise the event loop will never process the signals from the 
	//dropped conection
	QTest::qWait(serverTimeout+2000);

	channel_->sendCommand(command);
	QTest::qWait(50);
	QCOMPARE(incomingResponseSpy.count(),2);

	QCOMPARE(droppedConnectionSpy.count(),0);

}*/

//This tests the command channel when there is a streaming rsponse.
//This is also an implicit test of protocolResponse::read(), since it 
//requires the response to handle a multipart response.
/*void::TestCommandChannel::streamingResponse()
{
	channel_->pollingMode(false);

	qRegisterMetaType< QSharedPointer<Picto::ProtocolResponse> >("QSharedPointer<Picto::ProtocolResponse>");
	QSignalSpy spy(channel_, SIGNAL(incomingResponse(QSharedPointer<Picto::ProtocolResponse>)));	
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("GET /testMjpegStream HTTP/1.1"));

	channel_->sendCommand(command);

	//Wait for a bunch of partial responses to arrive
	QTest::qWait(500);

	qDebug()<<"Responses recieved: "<<spy.count();
	QVERIFY(spy.count() > 1);

	//grab the first response
	QList<QVariant> arguments = spy.takeFirst();
	QSharedPointer<Picto::ProtocolResponse> response = arguments[0].value<QSharedPointer<Picto::ProtocolResponse> >();

	//check the the content-type field is multipart...
	QCOMPARE(response->getFieldValue("Content-Type"), QString("multipart/x-mixed-replace; boundary=--pictoboundary"));
	QCOMPARE(response->getMultiPart(), Picto::MultiPartResponseType::MultiPartInitial);
	QCOMPARE(response->getMultiPartBoundary(), QString("--pictoboundary"));

	//grab all of the remaining responses
	while(spy.size() > 0)
	{
		arguments = spy.takeFirst();
		response = arguments[0].value<QSharedPointer<Picto::ProtocolResponse> >();

		qDebug()<<"Content-Length:"<<response->getFieldValue("Content-Length");
		QCOMPARE(response->getFieldValue("Content-Type"), QString("image/jpeg"));
		QCOMPARE(response->getMultiPart(), Picto::MultiPartResponseType::MultiPartPart);

		//The content size actually varies (since the content is a jpeg), so we
		//will simply confirm that there is "enough" content.
		//QVERIFY(response->getFieldValue("Content-Length").toInt()>2000);

	}

}*/

void TestCommandChannel::polledResponses_data()
{
	QTest::addColumn<int>("commandsToSend");
	QTest::newRow("Single response")<<1;
	QTest::newRow("Multiple responses")<<10;
}


//	This tests the command channel in producer mode.  The bad news is that there
//	is no way to get the server to send us commands, so we have to use our own server
//	for this test.  Because we're using a different server, we will also have to create
//	a new command channel.  Oh well...

//The test will pass if the following conditions are met:
//		1. The number of received commands equals the commandsToSend value 
//		2. Every incoming command has method POST
//		3. There is a single incoming response to the original TEST command
//		4. The incoming response is type 200 (OK)
//		4. The incoming response contains a report showing the following:
//			a. status="pass"
//			b. commandsSent=commandsToSend
//			c. responsesReceived=commandsToSend
/*void TestCommandChannel::polledResponses()
{
	QFETCH(int, commandsToSend);

	int testPort=24242; //hopefully nothing else is using this...

	ConsumerServer server(testPort);

	Picto::CommandChannel *testChannel;
	testChannel = new Picto::CommandChannel(createUuid(),"TESTCOMPONENT",QHostAddress::LocalHost,testPort);
	//testChannel->pollingMode(true);

	QSharedPointer<Picto::ProtocolCommand> command(
		new Picto::ProtocolCommand(QString("TEST /%1 PICTO/1.0").arg(commandsToSend)));
	testChannel->sendCommand(command);

	//sit in a loop polling for commands and sending responses
	//The wait is needed even though we're in polled mode, since the 
	//ConsumerServer uses the event loop
	int timeCounter=0;
	int commandCounter = 0;
	QSharedPointer<Picto::ProtocolCommand> incomingCommand;
	QSharedPointer<Picto::ProtocolResponse> ackResponse(
		new Picto::ProtocolResponse("commandchanel","PICTO", "1.0", Picto::ProtocolResponseType::OK));

	while(timeCounter < 1000 + commandsToSend*50) 
	{
		QTest::qWait(20);
		timeCounter += 20;
		while(testChannel->incomingCommandsWaiting())
		{
			incomingCommand = testChannel->getCommand();
			QString method = incomingCommand->getMethod();
			QCOMPARE(method, QString("POST"));
			
			commandCounter++;

			testChannel->sendResponse(ackResponse);
		}
	}

	QCOMPARE(commandCounter, commandsToSend);

	//Read the report sent in response to our initial "TEST" command
	QCOMPARE(testChannel->incomingResponsesWaiting(), 1);
	QSharedPointer<Picto::ProtocolResponse> incomingResponse;
	incomingResponse = testChannel->getResponse();
	QCOMPARE(incomingResponse->getResponseCode(), 200);	
	QString report = incomingResponse->getDecodedContent();

	QXmlStreamReader reader;
	reader.addData(report);
	QString status, commandsSent, responsesReceived;
	while(!reader.atEnd())
	{
		reader.readNext();
		if(reader.isStartElement() && reader.name() == "ConsumerServerReport")
		{
			status = reader.attributes().value("status").toString();
			commandsSent = reader.attributes().value("commandsSent").toString();
			responsesReceived = reader.attributes().value("responsesReceived").toString();
		}
	}

	QCOMPARE(commandsSent, QString("%1").arg(commandsToSend));
	QCOMPARE(responsesReceived, QString("%1").arg(commandsToSend));
	QCOMPARE(status, QString("pass"));


	delete testChannel;
}*/

/*void TestCommandChannel::eventDrivenResponses_data()
{
	QTest::addColumn<int>("commandsToSend");
	QTest::newRow("Single response")<<1;
	QTest::newRow("Multiple responses")<<10;
}*/

//This test is very similar to the polledResponses test, except that it uses the 
//event driven aspects of the command channel.

//The test will pass if the following conditions are met:
//		1. The number of received commands equals the commandsToSend value 
//		2. Every incoming command has method POST
//		3. There is a single incoming response to the original TEST command
//		4. The incoming response is type 200 (OK)
//		4. The incoming response contains a report showing the following:
//			a. status="pass"
//			b. commandsSent=commandsToSend
//			c. responsesReceived=commandsToSend

/*void TestCommandChannel::eventDrivenResponses()
{
	QFETCH(int, commandsToSend);

	int testPort=24242; //hopefully nothing else is using this...

	ConsumerServer server(testPort);

	Picto::CommandChannel *testChannel;
	testChannel = new Picto::CommandChannel(createUuid()"TESTCOMPONENT",QHostAddress::LocalHost,testPort);
	testChannel->pollingMode(false);

	qRegisterMetaType< QSharedPointer<Picto::ProtocolResponse> >("QSharedPointer<Picto::ProtocolResponse>");
	qRegisterMetaType< QSharedPointer<Picto::ProtocolCommand> >("QSharedPointer<Picto::ProtocolCommand>");

	QSignalSpy incomingResponseSpy(testChannel, SIGNAL(incomingResponse(QSharedPointer<Picto::ProtocolResponse>)));	
	QSignalSpy incomingCommandSpy(testChannel, SIGNAL(incomingCommand(QSharedPointer<Picto::ProtocolCommand>)));

	QSharedPointer<Picto::ProtocolCommand> command(
		new Picto::ProtocolCommand(QString("TEST /%1 PICTO/1.0").arg(commandsToSend)));
	testChannel->sendCommand(command);
	
	//sit in a loop polling for commands and sending responses
	//This is done manually, because connecting the signals and slots within a test is impossible
	//(any slot I create will be seen as a test case)
	int timeCounter=0;
	int commandCounter = 0;
	QSharedPointer<Picto::ProtocolCommand> incomingCommand;
	QSharedPointer<Picto::ProtocolResponse> ackResponse(
		new Picto::ProtocolResponse("commandchanel","PICTO", "1.0", Picto::ProtocolResponseType::OK));
	QList<QVariant> arguments;

	while(timeCounter < 1000 + commandsToSend*50) 
	{
		QTest::qWait(20);
		timeCounter += 20;
	
		while(incomingCommandSpy.size() > 0)
		{
			arguments = incomingCommandSpy.takeFirst();
			incomingCommand = arguments[0].value<QSharedPointer<Picto::ProtocolCommand> >();

			QString method = incomingCommand->getMethod();
			QCOMPARE(method, QString("POST"));
			
			commandCounter++;

			testChannel->sendResponse(ackResponse);
		}
	}
	
	QCOMPARE(commandCounter, commandsToSend);

	//Read the report sent in response to our initial "TEST" command
	QCOMPARE(incomingResponseSpy.count(), 1);

	QSharedPointer<Picto::ProtocolResponse> incomingResponse;
	arguments = incomingResponseSpy.takeFirst();
	incomingResponse = arguments[0].value<QSharedPointer<Picto::ProtocolResponse> >();
	QCOMPARE(incomingResponse->getResponseCode(), 200);	
	QString report = incomingResponse->getDecodedContent();

	QXmlStreamReader reader;
	reader.addData(report);
	QString status, commandsSent, responsesReceived;
	while(!reader.atEnd())
	{
		reader.readNext();
		if(reader.isStartElement() && reader.name() == "ConsumerServerReport")
		{
			status = reader.attributes().value("status").toString();
			commandsSent = reader.attributes().value("commandsSent").toString();
			responsesReceived = reader.attributes().value("responsesReceived").toString();
		}
	}

	QCOMPARE(commandsSent, QString("%1").arg(commandsToSend));
	QCOMPARE(responsesReceived, QString("%1").arg(commandsToSend));
	QCOMPARE(status, QString("pass"));


	delete testChannel;

}*/



//	The cleanup test case basically closes the channel, and
//	then deletes it.  It's impossible to check that this worked,
//	but we can read the debug stream to confirm that there aren't any 
//	messages.
void TestCommandChannel::cleanupTestCase()
{
	channel_->closeChannel();
	delete channel_;
}

