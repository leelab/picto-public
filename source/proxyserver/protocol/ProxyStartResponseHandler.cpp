#include "ProxyStartResponseHandler.h"
#include "NeuralDataAcqInterface.h"
#include "../../common/protocol/ProtocolCommand.h"
using namespace Picto;

ProxyStartResponseHandler::ProxyStartResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<CommandChannel> dataCommandChannel, QObject* acqPlugin):
StartResponseHandler(statusManager),
dataCommandChannel_(dataCommandChannel),
acqPlugin_(acqPlugin)
{}

bool ProxyStartResponseHandler::processResponse(QString directive)
{
	statusManager_->setStatus(running);
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	forever
	{
		//send a PUTDATA command to the server with the most recent behavioral data
		QSharedPointer<Picto::ProtocolResponse> dataResponse;
		QString dataCommandStr = "PUTDATA "+statusManager_->getName()+":"+ statusManager_->getStatusAsString() + " PICTO/1.0";
		QSharedPointer<Picto::ProtocolCommand> response(new Picto::ProtocolCommand(dataCommandStr));

		//set up XML writer
		QString xmlData;
		QSharedPointer<QXmlStreamWriter> writer(new QXmlStreamWriter(&xmlData));
		writer->setAutoFormatting(true);
		writer->writeStartDocument();
		writer->writeStartElement("Data");

		//Start writing the XML document
		//WE ARE NOT CURRENTLY USING THE DATA BELOW, SO ITS DISABLED, BUT WE WILL
		//NEED TO.  WHEN THIS HAPPENS, PUT IT IN A DATASTORE OF ITS OWN AND HANDLE
		//IT IN PUTDATA HANDLER ON SERVER.
		//writer->writeTextElement("device",iNDAcq->device());

		////check to see if our device is running
		//writer->writeStartElement("deviceStatus");
		//writer->writeCharacters("running");
		//writer->writeEndElement();
		//writer->writeTextElement("sampleRate",QString("%1").arg(iNDAcq->samplingRate()));

		//get the data from the neural acquisition device 
		QList<QSharedPointer<Picto::DataStore>> dataList = iNDAcq->dumpData();
		foreach(QSharedPointer<Picto::DataStore> data, dataList)
		{
			data->serializeAsXml(writer);
		}
		writer->writeEndElement(); // Data
		writer->writeEndDocument();
		
		response->setContent(xmlData.toUtf8());
		//response->setContentEncoding(Picto::ContentEncodingType::gzip); //ADD ENCODING!!!!!!!!!!!!!!!!
		response->setFieldValue("Content-Length",QString::number(xmlData.length()));
		
		if(dataList.size())
		{
			//qDebug("sent message");
			dataCommandChannel_->sendRegisteredCommand(response);
		}

		//check for and process responses
		QTime timer;
		timer.start();
		bool done = false;
		while(timer.elapsed() < 5000)
		{
			dataCommandChannel_->processResponses(5000);
			if(statusManager_->getStatus() <= stopped)
			{
				done = true;
				break;
			}
		}
		if(done)
			break;

	}
	// If we got here, we got a message that changed the state to stopped or less.
	return false;
}