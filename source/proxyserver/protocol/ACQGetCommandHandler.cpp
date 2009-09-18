#include "ACQGetCommandHandler.h"

#include <QDateTime>
#include <QNetworkInterface>
#include <QXmlStreamWriter>


#include "../../common/globals.h"

#include <windows.h>
#include "plexon.h"
#include "interfaces.h"

#define MAX_MAP_EVENTS_PER_READ 5000

//! Command handler for the ACQ protocol's GET command

/*!
 *  See the Trac wiki for a better description of this protocol. 
 *
 *  NOTE: The protocol grabs the full waveforms from the Plexon every single time.
 *  This may be a waste of time/data if we aren't using the waveforms, but given the
 *  way the Plexon server works (only giving us the data since the last time we
 *  asked for data), this seems to make the most sense.
 */

ACQGetCommandHandler::ACQGetCommandHandler(QObject *acqPlugin)
{
	iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin);
}

QSharedPointer<Picto::ProtocolResponse> ACQGetCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse("ACQ","1.0",Picto::ProtocolResponseType::OK));
	
	//set up XML writer
	QString xmlData;
	QXmlStreamWriter writer(&xmlData);

	writer.setAutoFormatting(true);
	writer.writeStartDocument();


	//Start writing the XML document
	writer.writeStartElement("device");
	writer.writeCharacters(iNDAcq->device());
	writer.writeEndElement();

	//check to see if our device is running
	writer.writeStartElement("deviceStatus");
	if(iNDAcq->getDeviceStatus() != NeuralDataAcqInterface::running)
	{
		writer.writeCharacters("not running");
		writer.writeEndElement();
		writer.writeEndDocument();
		response->setContent(xmlData.toUtf8());
		return response;
	}
	else
	{
		writer.writeCharacters("running");
		writer.writeEndElement();
	}

	//sampling rate
	if(command->getFieldValue("rate") == "1" || command->getFieldValue("all") == "1")
	{
		writer.writeStartElement("sampleRate");
		writer.writeCharacters(QString("%1").arg(iNDAcq->samplingRate()));
		writer.writeEndElement();
	}


	//get the data from the neural acquisition device 
	//(the plugin should have formatted it as XML already)
	xmlData.append(iNDAcq->dumpData());

	
	writer.writeEndDocument();
	
	response->setContent(xmlData.toUtf8());

	return response;
}
