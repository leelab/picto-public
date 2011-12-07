#include "ProxyServerProtocols.h"
#include "../../common/memleakdetect.h"

ProxyServerProtocols::ProxyServerProtocols()
{
}

void ProxyServerProtocols::addProtocol(QSharedPointer<Picto::Protocol> protocol)
{
	protocols[protocol->id()] = protocol;
}

QSharedPointer<Picto::Protocol> ProxyServerProtocols::getProtocol(QString protocolName)
{
	QSharedPointer<Picto::Protocol> protocol;

	std::map<QString,QSharedPointer<Picto::Protocol> >::const_iterator protocolIter = protocols.find(protocolName);

	if(protocolIter != protocols.end())
	{
		protocol = protocolIter->second;
	}

	return protocol;
}
