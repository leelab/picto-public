#include "ServerAppUpdateProtocol.h"

#include <QSharedPointer>

#include "../../common/common.h"

#include "GetFileCommandHandler.h"
#include "../../common/memleakdetect.h"

ServerAppUpdateProtocol::ServerAppUpdateProtocol()
{
	QSharedPointer<GetFileCommandHandler> getFileCommandHandler(new GetFileCommandHandler());

	addCommandHandler(getFileCommandHandler);
}

QString ServerAppUpdateProtocol::id()
{
	return QString("UPDATEPICTO");
}

QString ServerAppUpdateProtocol::version()
{
	return QString("1.0");
}
