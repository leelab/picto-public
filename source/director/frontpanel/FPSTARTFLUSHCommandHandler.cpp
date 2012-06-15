#include <QByteArray>
#include <QStringList>

#include <QTextStream>

#include "FPInterface.h"
#include "FPSTARTFLUSHCommandHandler.h"
#include "../../common/globals.h"



FPSTARTFLUSHCommandHandler::FPSTARTFLUSHCommandHandler()
{
}

QSharedPointer<Picto::ProtocolResponse> FPSTARTFLUSHCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
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
			out<<"\nStarting Flush on controller: "<<controller<<"\n\n";
			emit startFlush(controller);
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
