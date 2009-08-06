#include <QTcpSocket>
#include <QHostAddress>

#include <stdio.h>

int main(int argc, char *argv[])
{
	QTcpSocket tcpSocket;
	
	QHostAddress serverAddress;

	if(argc<2)
	{
		printf("Usage: NetworkTest ip_address\n");
	}
	
	serverAddress.setAddress(argv[1]);

	tcpSocket.connectToHost(serverAddress,42424);
	if(!tcpSocket.waitForConnected(10000))
	{
		printf("Couldn't connect\n");
	}

	tcpSocket.write("GET / PICTO/1.0\r\n\r\n");
	
	int bytesRead = 0;
	while(tcpSocket.waitForReadyRead(10000))
	{		
		QString result = tcpSocket.readAll();
		
		bytesRead += result.length();
		
		printf("Bytes read: %d\r",bytesRead);	
	}

	tcpSocket.disconnectFromHost();

	return 0;
}
