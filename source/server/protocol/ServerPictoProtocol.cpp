#include "ServerPictoProtocol.h"

#include <QSharedPointer>

#include "../../common/common.h"

#include "../../common/protocol/AnnounceCommandHandler.h"
#include "../../common/protocol/DiscoverCommandHandler.h"
#include "../../common/protocol/GetCommandHandler.h"
#include "../../common/protocol/TerminateCommandHandler.h"

#include "DirectorListCommandHandler.h"
#include "DirectorUpdateCommandHandler.h"
#include "StartsessionCommandHandler.h"
#include "StarttaskCommandHandler.h"
#include "TestCommandHandler.h"
#include "TrialCommandHandler.h"

ServerPictoProtocol::ServerPictoProtocol()
{
	QSharedPointer<Picto::AnnounceCommandHandler> announceCommandHandler(new Picto::AnnounceCommandHandler());
	QSharedPointer<Picto::DiscoverCommandHandler> discoverCommandHandler(new Picto::DiscoverCommandHandler());
	QSharedPointer<Picto::GetCommandHandler> getCommandHandler(new Picto::GetCommandHandler());
	QSharedPointer<Picto::TerminateCommandHandler> terminateCommandHandler(new Picto::TerminateCommandHandler());

	QSharedPointer<DirectorListCommandHandler> directorListCommandHandler(new DirectorListCommandHandler());
	QSharedPointer<DirectorUpdateCommandHandler> directorUpdateCommandHandler(new DirectorUpdateCommandHandler());
	QSharedPointer<StartsessionCommandHandler> startsessionCommandHandler(new StartsessionCommandHandler());
	QSharedPointer<StarttaskCommandHandler> starttaskCommandHandler(new StarttaskCommandHandler());
	QSharedPointer<TestCommandHandler> testCommandHandler(new TestCommandHandler());
	QSharedPointer<TrialCommandHandler> trialCommandHandler(new TrialCommandHandler());


	addCommandHandler(announceCommandHandler);
	addCommandHandler(discoverCommandHandler);
	addCommandHandler(getCommandHandler);
	addCommandHandler(terminateCommandHandler);

	addCommandHandler(directorListCommandHandler);
	addCommandHandler(directorUpdateCommandHandler);
	addCommandHandler(startsessionCommandHandler);
	addCommandHandler(starttaskCommandHandler);
	addCommandHandler(testCommandHandler);
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
