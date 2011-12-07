#include "ServerProtocols.h"
#include "../../common/memleakdetect.h"

ServerProtocols::ServerProtocols()
{
}

void ServerProtocols::addProtocol(QSharedPointer<Picto::Protocol> protocol)
{
	protocols[protocol->id()] = protocol;
}

QSharedPointer<Picto::Protocol> ServerProtocols::getProtocol(QString protocolName)
{
	QSharedPointer<Picto::Protocol> protocol;

	std::map<QString,QSharedPointer<Picto::Protocol> >::const_iterator protocolIter = protocols.find(protocolName);

	if(protocolIter != protocols.end())
	{
		protocol = protocolIter->second;
	}

	return protocol;
}
