#include <QByteArray>
#include <QStringList>

#include <QTextStream>

#include "FPInterface.h"
#include "FPREWARDCommandHandler.h"
#include "../../common/globals.h"


FPREWARDCommandHandler::FPREWARDCommandHandler()
{
}

/*! \brief Parses the input ProtocolCommand to check its syntax, extract the reward channel on which a reward should occur
 *	and emit a giveReward() signal accordingly.
 */
QSharedPointer<Picto::ProtocolResponse> FPREWARDCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QTextStream out(stdout);

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


	QStringList targetFields = command->getTarget().split('/',QString::SkipEmptyParts);
	if(targetFields.size()>1)
	{
		bool ok;
		int controller = targetFields[1].toInt(&ok);
		if(ok)
		{
			emit giveReward(controller);
			out<<"\n!Reward! Controller "<<controller<<"\n\n";
			return okResponse;
		}
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
