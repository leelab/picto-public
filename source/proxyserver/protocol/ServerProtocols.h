#ifndef _SERVERPROTOCOLS_H_
#define _SERVERPROTOCOLS_H_

#include <map>

#include <QString>
#include <QSharedPointer>

#include "../../common/common.h"
#include "../../common/protocol/Protocol.h"

struct ServerProtocols
{
public:
	ServerProtocols();
	
	void addProtocol(QSharedPointer<Picto::Protocol> protocol);
	QSharedPointer<Picto::Protocol> getProtocol(QString protocolName);

private:
	std::map<QString,QSharedPointer<Picto::Protocol> > protocols;
};

#endif
