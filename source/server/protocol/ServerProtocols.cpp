#include "ServerProtocols.h"
#include "../../common/memleakdetect.h"

ServerProtocols::ServerProtocols()
{
}

/*! \brief Adds a new Protocol to this object.  This Protocol will be used to handle incoming commands that reference it.
 */
void ServerProtocols::addProtocol(QSharedPointer<Picto::Protocol> protocol)
{
	protocols[protocol->id()] = protocol;
}

/*! \brief Returns the Protocol of the input type (protocolName).
 */
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
