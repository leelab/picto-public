#include <qDebug>
#include <QCoreApplication>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QHostAddress>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "../../common/globals.h"

#include "TestCommandHandler.h"
#include "../datacollection/alignmenttool.h"
#include "../../common/memleakdetect.h"

TestCommandHandler::TestCommandHandler()
{
}

/*! \brief Returns an OK response to any input command.  This can be used by clients as a sort of sanity check, like
 *	pinging google.
 */
QSharedPointer<Picto::ProtocolResponse> TestCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	//create an "OK" response
	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName,"PICTO","1.0",Picto::ProtocolResponseType::OK));
	return response;
}