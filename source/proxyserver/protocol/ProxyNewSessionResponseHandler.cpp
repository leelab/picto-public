#include <QApplication>
#include "ProxyNewSessionResponseHandler.h"
#include "NeuralDataAcqInterface.h"
#include "../../common/protocol/ProtocolCommand.h"
#include "../../common/memleakdetect.h"
#include "../network/proxystatusmanager.h"
using namespace Picto;

ProxyNewSessionResponseHandler::ProxyNewSessionResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<CommandChannel> dataCommandChannel):
NewSessionResponseHandler(statusManager,dataCommandChannel),
dataCommandChannel_(dataCommandChannel)
{}

bool ProxyNewSessionResponseHandler::processResponse(QString directive)
{
	NewSessionResponseHandler::processResponse(directive);
	Q_ASSERT(!statusManager_.isNull());
	statusManager_.toStrongRef()->setStatus(running);
	NeuralDataAcqInterface *iNDAcq;
	bool firstTime = true;
	while(statusManager_.toStrongRef()->getStatus() > ending 
			&& !statusManager_.toStrongRef()->exitTriggered())
	{
		iNDAcq = statusManager_.toStrongRef().staticCast<ProxyStatusManager>()->getPlugin();
		if(iNDAcq)
		{
			//If this is the first time we're requesting data, throw out any data that was acquired so far
			//because it comes from before this session started.
			if(firstTime)
			{
				if(!iNDAcq->acqDataAfterNow())
				{
					//check for and process responses
					dataCommandChannel_->assureConnection();
					dataCommandChannel_->processResponses(0);
					QCoreApplication::processEvents();
					continue;
				}
				firstTime = false;
			}

			//get the data from the neural acquisition device 
			QList<QSharedPointer<Picto::DataUnit>> dataList = iNDAcq->dumpData();
			while(dataList.size())
			{
				//Generate PUTDATA commands with limited length until we're out of data
				QSharedPointer<Picto::ProtocolResponse> dataResponse;
				QString dataCommandStr = "PUTDATA "+statusManager_.toStrongRef()->getName()+":"+ statusManager_.toStrongRef()->getStatusAsString() + " PICTO/1.0";
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
				//Limit each message length to 100000 characters + up to one datastore length
				while(dataList.size() && (xmlData.length() < 100000))
				{
					dataList.takeFirst()->toXml(writer);
				}
				writer->writeEndElement(); // Data
				writer->writeEndDocument();
				
				response->setContent(xmlData.toUtf8());
				//response->setContentEncoding(Picto::ContentEncodingType::gzip); //ADD ENCODING!!!!!!!!!!!!!!!!
				response->setFieldValue("Content-Length",QString::number(xmlData.length()));
				
				//qDebug("sent message");
				dataCommandChannel_->sendRegisteredCommand(response);
			}
		}

		//check for and process responses
		dataCommandChannel_->assureConnection(5000);	//If a disconnect occured, this will allow up to 5 seconds for a reconnect.
		dataCommandChannel_->processResponses(0);
		QCoreApplication::processEvents();
	}
	// If we got here, we got a message that changed the state to stopped or less or the 
	// proxy closed.
	return false;
}