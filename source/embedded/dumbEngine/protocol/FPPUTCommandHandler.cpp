#include <QByteArray>
#include <QStringList>
#include <QHostAddress>

#include <QTextStream>

#include "FPPUTCommandHandler.h"


FPPUTCommandHandler::FPPUTCommandHandler()
{
}

QSharedPointer<Picto::ProtocolResponse> FPPUTCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(
		new Picto::ProtocolResponse("PICTO",
									"1.0",
									Picto::ProtocolResponseType::OK));

	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(
		new Picto::ProtocolResponse("PICTO",
									"1.0",
									Picto::ProtocolResponseType::NotFound));

	QSharedPointer<Picto::ProtocolResponse> badRequestResponse(
		new Picto::ProtocolResponse("PICTO",
									"1.0",
									Picto::ProtocolResponseType::BadRequest));
	QString commandContent;
	QByteArray responseContent;
	if(command->getTarget() == "/IP")
	{
		commandContent = command->getContent();
		QHostAddress newAddr;
		if(newAddr.setAddress(commandContent))
		{
			eng->setIP(newAddr);
			okResponse->setContent(commandContent.toUtf8());
			return okResponse;
		}
		else
			return badRequestResponse;

	}
	/*! \todo There is no error checking being done on this string.  Since the string is
	 *   coming from the front panel, we are assuming that the front panel is passing
	 *   us a safe value.  We might want to do a bit of error checking here, just 
	 *   to be safe though.
	 */
	else if(command->getTarget() == "/boxname")
	{
		commandContent = command->getContent();
		eng->setName(commandContent);
		okResponse->setContent(commandContent.toUtf8());
		return okResponse;
	}
	else if(command->getTarget().startsWith("/reward"))
	{
		QStringList targetFields = command->getTarget().split('/',QString::SkipEmptyParts);
		if(targetFields.size()>2 && targetFields[2] == "duration")
		{
			bool ok;
			int controller = targetFields[1].toInt(&ok);
			if(ok)
			{
				commandContent = command->getContent();
				int newDur = commandContent.toInt(&ok);
				if(!ok)
					return badRequestResponse;
				eng->setRewardDuration(controller, newDur);
				okResponse->setContent(commandContent.toUtf8());
				return okResponse;
			}
			else
				return notFoundResponse;
		}
		if(targetFields.size()>2 && targetFields[2] == "flushduration")
		{
			bool ok;
			int controller = targetFields[1].toInt(&ok);
			if(ok)
			{
				commandContent = command->getContent();
				int newDur = commandContent.toInt(&ok);
				if(!ok)
					return badRequestResponse;
				eng->setFlushDuration(controller, newDur);
				okResponse->setContent(commandContent.toUtf8());
				return okResponse;
			}
			else
				return notFoundResponse;
		}
		else
			return notFoundResponse;
	}
	else
	{
		return notFoundResponse;
	}

}
