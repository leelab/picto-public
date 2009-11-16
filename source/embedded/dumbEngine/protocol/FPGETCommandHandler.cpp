#include <QByteArray>
#include <QStringList>

#include <QTextStream>

#include "FPGETCommandHandler.h"
#include "../../common/globals.h"


FPGETCommandHandler::FPGETCommandHandler()
{
}

QSharedPointer<Picto::ProtocolResponse> FPGETCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QSharedPointer<Picto::ProtocolResponse> response(
		new Picto::ProtocolResponse(Picto::Names->directorAppName,
									"PICTO",
									"1.0",
									Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(
		new Picto::ProtocolResponse(Picto::Names->directorAppName,
									"PICTO",
									"1.0",
									Picto::ProtocolResponseType::NotFound));

	QByteArray content;

	if(command->getTarget() == "/IP")
	{
		content = eng->getIP().toString().toUtf8();
	}
	else if(command->getTarget() == "/boxname")
	{
		content = eng->getName().toUtf8();
	}
	else if(command->getTarget().startsWith("/reward"))
	{
		QTextStream out(stdout);

		QStringList targetFields = command->getTarget().split('/',QString::SkipEmptyParts);
		if(targetFields.size()>2 && targetFields[2] == "duration")
		{
			bool ok;
			int controller = targetFields[1].toInt(&ok);
			if(ok)
				content = QString("%1").arg(eng->getRewardDuration(controller)).toUtf8();
			else
			{
				out<<"Reward controller not an int\n";
				return notFoundResponse;
			}
		}
		else if(targetFields.size()>2 && targetFields[2] == "flushduration")
		{
			bool ok;
			int controller = targetFields[1].toInt(&ok);
			if(ok)
				content = QString("%1").arg(eng->getFlushDuration(controller)).toUtf8();
			else
			{
				out<<"Reward controller not an int\n";
				return notFoundResponse;
			}
		}
		else if(targetFields.size()>2 && targetFields[2] == "flushtimeremain")
		{
			bool ok;
			int controller = targetFields[1].toInt(&ok);
			if(ok)
				content = QString("%1").arg(eng->getFlushTimeRemain(controller)).toUtf8();
			else
			{
				out<<"Reward controller not an int\n";
				return notFoundResponse;
			}
		}
		else
		{
			out<<"Bad split.\n";
			return notFoundResponse;
		}
	}
	else
	{
		return notFoundResponse;
	}

	response->setContent(content);

	return response;
}
