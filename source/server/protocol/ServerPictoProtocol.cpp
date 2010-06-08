#include "ServerPictoProtocol.h"

#include <QSharedPointer>

#include "../../common/common.h"

#include "../../common/protocol/AnnounceCommandHandler.h"
#include "../../common/protocol/DiscoverCommandHandler.h"
#include "../../common/protocol/GetCommandHandler.h"
#include "../../common/protocol/TerminateCommandHandler.h"

#include "PutDataCommandHandler.h"
#include "DirectorListCommandHandler.h"
#include "DirectorUpdateCommandHandler.h"
#include "EndsessionCommandHandler.h"
#include "StartsessionCommandHandler.h"
#include "TaskCommandHandler.h"
#include "TestCommandHandler.h"
#include "TrialCommandHandler.h"

ServerPictoProtocol::ServerPictoProtocol()
{
	QSharedPointer<Picto::AnnounceCommandHandler> announceCommandHandler(new Picto::AnnounceCommandHandler());
	QSharedPointer<Picto::DiscoverCommandHandler> discoverCommandHandler(new Picto::DiscoverCommandHandler());
	QSharedPointer<Picto::GetCommandHandler> getCommandHandler(new Picto::GetCommandHandler());
	QSharedPointer<Picto::TerminateCommandHandler> terminateCommandHandler(new Picto::TerminateCommandHandler());

	QSharedPointer<PutDataCommandHandler> putDataCommandHandler(new PutDataCommandHandler());
	QSharedPointer<DirectorListCommandHandler> directorListCommandHandler(new DirectorListCommandHandler());
	QSharedPointer<DirectorUpdateCommandHandler> directorUpdateCommandHandler(new DirectorUpdateCommandHandler());
	QSharedPointer<EndsessionCommandHandler> endsessionCommandHandler(new EndsessionCommandHandler());
	QSharedPointer<StartsessionCommandHandler> startsessionCommandHandler(new StartsessionCommandHandler());
	QSharedPointer<TaskCommandHandler> taskCommandHandler(new TaskCommandHandler());
	QSharedPointer<TestCommandHandler> testCommandHandler(new TestCommandHandler());
	QSharedPointer<TrialCommandHandler> trialCommandHandler(new TrialCommandHandler());


	addCommandHandler(announceCommandHandler);
	addCommandHandler(discoverCommandHandler);
	addCommandHandler(getCommandHandler);
	addCommandHandler(terminateCommandHandler);

	addCommandHandler(putDataCommandHandler);
	addCommandHandler(directorListCommandHandler);
	addCommandHandler(directorUpdateCommandHandler);
	addCommandHandler(endsessionCommandHandler);
	addCommandHandler(startsessionCommandHandler);
	addCommandHandler(taskCommandHandler);
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
