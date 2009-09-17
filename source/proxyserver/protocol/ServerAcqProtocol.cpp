#include "ServerAcqProtocol.h"
#include "ACQGetCommandHandler.h"

#include <QSharedPointer>

#include "../../common/common.h"


ServerAcqProtocol::ServerAcqProtocol(QObject *acqPlugin)
{
	QSharedPointer<ACQGetCommandHandler> getCommandHandler(new ACQGetCommandHandler(acqPlugin));

	addCommandHandler(getCommandHandler);
}

QString ServerAcqProtocol::id()
{
	return QString("ACQ");
}

QString ServerAcqProtocol::version()
{
	return QString("1.0");
}
