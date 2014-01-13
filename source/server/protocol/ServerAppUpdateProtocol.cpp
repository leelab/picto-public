#include "ServerAppUpdateProtocol.h"

#include <QSharedPointer>

#include "../../common/common.h"

#include "GetFileCommandHandler.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a ServerAppUpdateProtocol object which includes a single ProtocolCommandHandler,
 *	the GetFileCommandHandler that is used for older Picto version to download the files for a new
 *	Picto installation.
*/
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
