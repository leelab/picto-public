#include <QByteArray>
#include <QStringList>

#include <QTextStream>

#include "FPSTARTFLUSHCommandHandler.h"


FPSTARTFLUSHCommandHandler::FPSTARTFLUSHCommandHandler()
{
}

QSharedPointer<Picto::ProtocolResponse> FPSTARTFLUSHCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QTextStream out(stdout);

	QSharedPointer<Picto::ProtocolResponse> okResponse(
		new Picto::ProtocolResponse("PICTO",
									"1.0",
									Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(
		new Picto::ProtocolResponse("PICTO",
									"1.0",
									Picto::ProtocolResponseType::NotFound));


	QStringList targetFields = command->getTarget().split('/',QString::SkipEmptyParts);
	if(targetFields.size()>1)
	{
		bool ok;
		int controller = targetFields[1].toInt(&ok);
		if(ok)
		{
			/*! \todo we should actually give a reward here when this is used with 
			 *  the real engine...
			 */
			out<<"\nStarting Flush on controller: "<<controller<<"\n\n";
			eng->startFlush(controller);
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
