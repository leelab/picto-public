#include "UpdateListener.h"

#include <QtNetwork>

UpdateListener::UpdateListener(QObject *parent) :
  QTcpServer(parent)
{
	QHostAddress serverAddress(QHostAddress::Any);

	listen(serverAddress, 42000);
}

void UpdateListener::incomingConnection(int socketDescriptor)
{
	QTcpSocket tcpSocket;
    if (!tcpSocket.setSocketDescriptor(socketDescriptor))
    {
        return;
    }

	char receivedPositionScript[10000];
	char readCharacter = 0;
	unsigned int bufferPosition = 0;

	do
	{
		if(tcpSocket.waitForReadyRead(2000))
		{
			while(!tcpSocket.atEnd())
			{
				tcpSocket.getChar(&readCharacter);
				receivedPositionScript[bufferPosition] = readCharacter;
				bufferPosition++;
			}
		}
		else
		{
			readCharacter = 0x0A;
		}
	} while (readCharacter != 0x0A && readCharacter != 0x0D && bufferPosition < sizeof(receivedPositionScript));
	receivedPositionScript[bufferPosition] = '\0';

	ddrawObject.setScript(receivedPositionScript);

	tcpSocket.disconnectFromHost();
    tcpSocket.waitForDisconnected();
}
