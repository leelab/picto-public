#include "GetCommandHandler.h"
#include "../common/globals.h"
#include "../memleakdetect.h"

namespace Picto {

GetCommandHandler::GetCommandHandler()
{
}

QSharedPointer<ProtocolResponse> GetCommandHandler::processCommand(QSharedPointer<ProtocolCommand> command)
{
	//QSharedPointer<ProtocolResponse> response = new ProtocolResponse("PICTO","1.0",ProtocolResponseType::NotImplemented);
	QSharedPointer<ProtocolResponse> response(new ProtocolResponse(Picto::Names->serverAppName,"PICTO","1.0",ProtocolResponseType::OK));

	int streamState = command->getFieldValue("X-PictoStreamState").toInt();

	response->setShouldStream(true);
	response->setContent(QString("%1\r\n").arg(++streamState).toUtf8());
	response->addField("X-PictoStreamState",QString("%1").arg(streamState));

	return response;
}

}; //namespace Picto
