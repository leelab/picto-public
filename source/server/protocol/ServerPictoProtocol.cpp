#include "ServerPictoProtocol.h"

#include <QSharedPointer>

#include "../../common/common.h"

#include "../../common/protocol/AnnounceCommandHandler.h"
#include "../../common/protocol/DiscoverCommandHandler.h"
#include "../../common/protocol/GetCommandHandler.h"
#include "../../common/protocol/TerminateCommandHandler.h"

#include "DirectorListCommandHandler.h"
#include "ComponentUpdateCommandHandler.h"
#include "EndsessionCommandHandler.h"
#include "GetDataCommandHandler.h"
#include "JoinsessionCommandHandler.h"
#include "ProxyListCommandHandler.h"
#include "PutDataCommandHandler.h"
#include "StartsessionCommandHandler.h"
#include "TaskCommandHandler.h"
#include "TestCommandHandler.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a ServerPictoProtocol object which includes ProtocolCommandHandlers to handle
 *	all of the various commands that allow operators to control/view experimental runs and allow Session Components 
 *	(Director, Proxy) to upload data to the server.
*/
ServerPictoProtocol::ServerPictoProtocol()
{
	QSharedPointer<Picto::AnnounceCommandHandler> announceCommandHandler(new Picto::AnnounceCommandHandler());
	QSharedPointer<Picto::DiscoverCommandHandler> discoverCommandHandler(new Picto::DiscoverCommandHandler());
	QSharedPointer<Picto::GetCommandHandler> getCommandHandler(new Picto::GetCommandHandler());
	QSharedPointer<Picto::TerminateCommandHandler> terminateCommandHandler(new Picto::TerminateCommandHandler());

	QSharedPointer<DirectorListCommandHandler> directorListCommandHandler(new DirectorListCommandHandler());
	QSharedPointer<ComponentUpdateCommandHandler> directorUpdateCommandHandler(new ComponentUpdateCommandHandler());
	QSharedPointer<EndsessionCommandHandler> endsessionCommandHandler(new EndsessionCommandHandler());
	QSharedPointer<GetDataCommandHandler> getDataCommandHandler(new GetDataCommandHandler());
	QSharedPointer<JoinsessionCommandHandler> joinsessionCommandHandler(new JoinsessionCommandHandler());
	QSharedPointer<ProxyListCommandHandler> proxyListCommandHandler(new ProxyListCommandHandler());
	QSharedPointer<PutDataCommandHandler> putDataCommandHandler(new PutDataCommandHandler());
	QSharedPointer<StartsessionCommandHandler> startsessionCommandHandler(new StartsessionCommandHandler());
	QSharedPointer<TaskCommandHandler> taskCommandHandler(new TaskCommandHandler());
	QSharedPointer<TestCommandHandler> testCommandHandler(new TestCommandHandler());


	addCommandHandler(announceCommandHandler);
	addCommandHandler(discoverCommandHandler);
	addCommandHandler(getCommandHandler);
	addCommandHandler(terminateCommandHandler);

	addCommandHandler(directorListCommandHandler);
	addCommandHandler(directorUpdateCommandHandler);
	addCommandHandler(endsessionCommandHandler);
	addCommandHandler(getDataCommandHandler);
	addCommandHandler(joinsessionCommandHandler);
	addCommandHandler(proxyListCommandHandler);
	addCommandHandler(putDataCommandHandler);
	addCommandHandler(startsessionCommandHandler);
	addCommandHandler(taskCommandHandler);
	addCommandHandler(testCommandHandler);
}

QString ServerPictoProtocol::id()
{
	return QString("PICTO");
}

QString ServerPictoProtocol::version()
{
	return QString("1.0");
}
