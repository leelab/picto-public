#include "ServerPictoProtocol.h"

#include <QSharedPointer>

#include "../../common/common.h"

#include "../../common/protocol/AnnounceCommandHandler.h"
#include "../../common/protocol/DiscoverCommandHandler.h"
#include "../../common/protocol/GetCommandHandler.h"
#include "../../common/protocol/TerminateCommandHandler.h"
#include "DebugCommandHandler.h"

ServerPictoProtocol::ServerPictoProtocol()
{
	QSharedPointer<Picto::AnnounceCommandHandler> announceCommandHandler(new Picto::AnnounceCommandHandler());
	QSharedPointer<Picto::DiscoverCommandHandler> discoverCommandHandler(new Picto::DiscoverCommandHandler());
	QSharedPointer<Picto::GetCommandHandler> getCommandHandler(new Picto::GetCommandHandler());
	QSharedPointer<Picto::TerminateCommandHandler> terminateCommandHandler(new Picto::TerminateCommandHandler());
	QSharedPointer<DebugCommandHandler> debugCommandHandler(new DebugCommandHandler());


	addCommandHandler(announceCommandHandler);
	addCommandHandler(discoverCommandHandler);
	addCommandHandler(getCommandHandler);
	addCommandHandler(terminateCommandHandler);
	addCommandHandler(debugCommandHandler);
}

QString ServerPictoProtocol::id()
{
	return QString("PICTO");
}

QString ServerPictoProtocol::version()
{
	return QString("1.0");
}
