#include "ServerPictoProtocol.h"

#include <QSharedPointer>

#include "../../common/common.h"

#include "../../common/protocol/AnnounceCommandHandler.h"
#include "../../common/protocol/DiscoverCommandHandler.h"
#include "../../common/protocol/GetCommandHandler.h"
#include "../../common/protocol/TerminateCommandHandler.h"
#include "TrialCommandHandler.h"
#include "DirectorUpdateCommandHandler.h"
#include "DirectorListCommandHandler.h"
#include "TestCommandHandler.h"

ServerPictoProtocol::ServerPictoProtocol()
{
	QSharedPointer<Picto::AnnounceCommandHandler> announceCommandHandler(new Picto::AnnounceCommandHandler());
	QSharedPointer<Picto::DiscoverCommandHandler> discoverCommandHandler(new Picto::DiscoverCommandHandler());
	QSharedPointer<Picto::GetCommandHandler> getCommandHandler(new Picto::GetCommandHandler());
	QSharedPointer<Picto::TerminateCommandHandler> terminateCommandHandler(new Picto::TerminateCommandHandler());
	QSharedPointer<TestCommandHandler> testCommandHandler(new TestCommandHandler());
	QSharedPointer<DirectorUpdateCommandHandler> directorUpdateCommandHandler(new DirectorUpdateCommandHandler());
	QSharedPointer<DirectorListCommandHandler> directorListCommandHandler(new DirectorListCommandHandler());
	QSharedPointer<TrialCommandHandler> trialCommandHandler(new TrialCommandHandler());


	addCommandHandler(announceCommandHandler);
	addCommandHandler(discoverCommandHandler);
	addCommandHandler(getCommandHandler);
	addCommandHandler(terminateCommandHandler);
	addCommandHandler(testCommandHandler);
	addCommandHandler(directorUpdateCommandHandler);
	addCommandHandler(directorListCommandHandler);
	addCommandHandler(trialCommandHandler);
}

QString ServerPictoProtocol::id()
{
	return QString("PICTO");
}

QString ServerPictoProtocol::version()
{
	return QString("1.0");
}
