#include "ProxyServerAcqProtocol.h"
#include "ACQGetCommandHandler.h"

#include <QSharedPointer>

#include "../../common/common.h"
#include "../../common/memleakdetect.h"


ProxyServerAcqProtocol::ProxyServerAcqProtocol(QObject *acqPlugin)
{
	QSharedPointer<ACQGetCommandHandler> getCommandHandler(new ACQGetCommandHandler(acqPlugin));

	addCommandHandler(getCommandHandler);
}

QString ProxyServerAcqProtocol::id()
{
	return QString("ACQ");
}

QString ProxyServerAcqProtocol::version()
{
	return QString("1.0");
}
