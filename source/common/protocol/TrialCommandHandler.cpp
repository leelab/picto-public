#include "TrialCommandHandler.h"

#include "../../common/globals.h"

#include <QXmlStreamReader>
#include <QFile>

namespace Picto {

TrialCommandHandler::TrialCommandHandler()
{
}

/*! \brief hands a TRIAL command
 *
 *	Since the TRIAL command doesn't expect a response, we return a null Response
 */
QSharedPointer<Picto::ProtocolResponse> TrialCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	//If there's an unrecognized target, then something has gone horribly wrong
	Q_ASSERT(command->getTarget() == "/start" || command->getTarget() == "/end");

	//Extract the content
	QByteArray content = command->getContent();
	QXmlStreamReader xmlReader(content);

	double time = -1.0;
	int eventCode = -1;
	int trialNum = -1;

	while(!xmlReader.atEnd())
	{
		QString xmlName = xmlReader.name().toString();

		if(!xmlReader.isStartElement())
		{
			xmlReader.readNext();
			continue;
		}

		if(xmlReader.name() == "Time")
		{
			time = xmlReader.readElementText().toDouble();
		}
		else if(xmlReader.name() == "EventCode")
		{
			eventCode = xmlReader.readElementText().toInt();
		}
		else if(xmlReader.name() == "TrialNum")
		{
			trialNum = xmlReader.readElementText().toInt();
		}
		xmlReader.readNext();
	}

	if(xmlReader.hasError())
	{
		QString adf = xmlReader.errorString();
		QSharedPointer<ProtocolResponse> response(new ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",ProtocolResponseType::BadRequest));
		return response;
	}


	if(time < 0 || eventCode < 0 || trialNum <0)
	{
		QSharedPointer<ProtocolResponse> response(new ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",ProtocolResponseType::BadRequest));
		return response;
	}

	//! \todo Actually implement this stuff...
	//For the moment we are simply going to print out the start trial info...
	QFile outFile("TRIALevents.txt");
	outFile.open(QIODevice::Append);

	if(command->getTarget() == "/start")
	{
		outFile.write("Trial Start\n");
	}
	else if(command->getTarget() == "/end")
	{
		outFile.write("Trial End\n");
	}

	outFile.write(QString(" Time: %1\n").arg(time).toAscii());
	outFile.write(QString(" Trial number: %1\n").arg(trialNum).toAscii());
	outFile.write(QString(" Event code: %1\n\n").arg(eventCode).toAscii());

	outFile.close();

	QSharedPointer<ProtocolResponse> response(new ProtocolResponse(Picto::Names->serverAppName, "PICTO","1.0",ProtocolResponseType::OK));

	return response;
}

}//namespace Picto {
