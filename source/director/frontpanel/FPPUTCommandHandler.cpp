#include <QByteArray>
#include <QStringList>
#include <QHostAddress>

#include <QTextStream>

#include "FPInterface.h"
#include "FPPUTCommandHandler.h"
#include "../../common/globals.h"


FPPUTCommandHandler::FPPUTCommandHandler()
{
}

/*! \brief Parses the input ProtocolCommand to check its syntax and set any requested values.
 *	\details Fields that can he set include PictoBox Name, Reward duration, Flush duration.
 *	The actual implementation of the input requests are performed using signals.  It is the 
 *	responsibility of the object that created this one to connect to the signals and make them
 *	do what they are supposed to do.
 */
QSharedPointer<Picto::ProtocolResponse> FPPUTCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QSharedPointer<Picto::ProtocolResponse> okResponse(
		new Picto::ProtocolResponse(Picto::Names->directorAppName,
									"PICTO",
									"1.0",
									Picto::ProtocolResponseType::OK));
	okResponse->setFieldValue("Command-ID",command->getFieldValue("Command-ID"));

	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(
		new Picto::ProtocolResponse(Picto::Names->directorAppName,
									"PICTO",
									"1.0",
									Picto::ProtocolResponseType::NotFound));
	notFoundResponse->setFieldValue("Command-ID",command->getFieldValue("Command-ID"));

	QSharedPointer<Picto::ProtocolResponse> badRequestResponse(
		new Picto::ProtocolResponse(Picto::Names->directorAppName,
									"PICTO",
									"1.0",
									Picto::ProtocolResponseType::BadRequest));
	badRequestResponse->setFieldValue("Command-ID",command->getFieldValue("Command-ID"));

	QString commandContent;
	QByteArray responseContent;
	if(command->getTarget() == "/IP")
	{
		commandContent = command->getContent();
		QHostAddress newAddr;
		if(newAddr.setAddress(commandContent))
		{
			//fpInterface_.toStrongRef()->setIP(newAddr);
			qDebug("IP ADDRESS CHANGE REQUESTED! THERE IS NO WAY I'M IMPLEMENTING THIS RIGHT NOW.");
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
		emit setName(commandContent);
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
				emit setRewardDuration(controller, newDur);
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
				emit setFlushDuration(controller, newDur);
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
