#ifndef UPDATELISTENER_H
#define UPDATELISTENER_H

#include <QtNetwork>

#include "ddrawobject.h"

class UpdateListener : public QTcpServer
{
	Q_OBJECT

public:
	UpdateListener(QObject *parent = 0);

private:
	void incomingConnection(int socketDescriptor);
	DDrawObject ddrawObject;
};

#endif