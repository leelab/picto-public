#include "TestBasicProtocolCommands.h"

#include <QString>
#include <QTcpSocket>
#include <QHostAddress>

TestBasicProtocolCommands::TestBasicProtocolCommands(QHostAddress _serverAddress) :
	serverAddress_(_serverAddress),
	port_(42424)
{
}

void TestBasicProtocolCommands::testQueryResponse(QString query, QString response)
{
	QTcpSocket tcpSocket;

	tcpSocket.connectToHost(serverAddress_,port_);

	if(!tcpSocket.waitForConnected(10000))
	{
		QFAIL("Unable to establish a connection");
	}

	tcpSocket.write(query.toAscii());
	if(tcpSocket.waitForReadyRead(10000))
	{
		QString result = tcpSocket.readAll();
		QStringList resultLines = result.split("\r\n");
		QCOMPARE(resultLines.first(),response);
	}
	else
	{
		tcpSocket.disconnectFromHost();
		QFAIL("Timeout while waiting for server response");
	}

	tcpSocket.disconnectFromHost();
}

void TestBasicProtocolCommands::httpGet_data()
{
	QTest::addColumn<QString>("method");
	QTest::addColumn<QString>("target");
	QTest::addColumn<QString>("protocol");

	QTest::newRow("/") << "GET" << "/" << "HTTP/1.1";
	QTest::newRow("/DoesNotExist") << "GET" << "/DoesNotExist" << "HTTP/1.1";
}

void TestBasicProtocolCommands::httpGet()
{
	QFETCH(QString, method);
	QFETCH(QString, target);
	QFETCH(QString, protocol);

	QString request = method + " " + target + " " + protocol + "\r\n\r\n";
	QString expected;
	
	if(target == "/")
	{
		expected = "HTTP/1.1 200 OK";
	}
	else
	{
		//expected = "HTTP/1.1 400 Bad Request";
		expected = "HTTP/1.1 404 Resource Not Found";
	}

	testQueryResponse(request, expected);
}

void TestBasicProtocolCommands::pictoGet_data()
{
	QTest::addColumn<QString>("method");
	QTest::addColumn<QString>("target");
	QTest::addColumn<QString>("protocol");

	QTest::newRow("/") << "GET" << "/" << "PICTO/1.1";
	QTest::newRow("/DoesNotExist") << "GET" << "/DoesNotExist" << "PICTO/1.1";
}

void TestBasicProtocolCommands::pictoGet()
{
	QFETCH(QString, method);
	QFETCH(QString, target);
	QFETCH(QString, protocol);

	QString request = method + " " + target + " " + protocol + "\r\n\r\n";
	QString expected;
	
	if(target == "/")
	{
		expected = "PICTO/1.0 200 OK";
	}
	else
	{
		expected = "PICTO/1.0 404 Not Found";
	}

	testQueryResponse(request, expected);
}

void TestBasicProtocolCommands::pictoTerminate()
{
}

void TestBasicProtocolCommands::malformedRequest_data()
{
}

void TestBasicProtocolCommands::malformedRequest()
{
}

void TestBasicProtocolCommands::incompleteRequest()
{
}

void TestBasicProtocolCommands::unknownRequest_data()
{
	QTest::addColumn<QString>("method");
	QTest::addColumn<QString>("target");
	QTest::addColumn<QString>("protocol");

	QTest::newRow("ASCII PICTO command") << "NOTIMPLEMENTED" << "/" << "PICTO/1.0";
	QTest::newRow("BINARY PICTO command") << "\0x07\0xFE" << "\0x04" << "PICTO/1.0";
}

void TestBasicProtocolCommands::unknownRequest()
{
	QFETCH(QString, method);
	QFETCH(QString, target);
	QFETCH(QString, protocol);

	QString request = method + " " + target + " " + protocol + "\r\n\r\n";
	QString expected = "PICTO/1.0 501 Not Implemented";

	testQueryResponse(request, expected);
}


/*
void TestBasicProtocolCommands::multipleConnectionBenchmark_data()
{
	QTest::addColumn<int>("numPreExistingConnections");

	QTest::newRow("1 user") << 1;
	QTest::newRow("2 users") << 2;
	QTest::newRow("5 users") << 5;
	QTest::newRow("10 users") << 10;
	QTest::newRow("25 users") << 25;
	QTest::newRow("100 users") << 100;
}

void TestBasicProtocolCommands::multipleConnectionBenchmark()
{
	QFETCH(int, numPreExistingConnections);

	QList<QTcpSocket *> tcpSockets;

	for(int i = 0; i < numPreExistingConnections; i++)
	{
		QTcpSocket * tcpSocket = new QTcpSocket();

		tcpSocket->connectToHost(serverAddress_,port_);
		tcpSocket->waitForConnected(10000);
		tcpSockets.append(tcpSocket);
	}

	QTcpSocket testTcpSocket;

	QBENCHMARK {
		testTcpSocket.connectToHost(serverAddress_,port_);
		testTcpSocket.waitForConnected(10000);
		testTcpSocket.disconnectFromHost();
		if(testTcpSocket.state() != QTcpSocket::UnconnectedState) testTcpSocket.waitForDisconnected(1000);
	}

	while(!tcpSockets.isEmpty())
	{
		QTcpSocket * tcpSocket = tcpSockets.takeLast();

		tcpSocket->disconnectFromHost();
		if(tcpSocket->state() != QTcpSocket::UnconnectedState) tcpSocket->waitForDisconnected(1000);

		delete tcpSocket;
	}
}
*/
