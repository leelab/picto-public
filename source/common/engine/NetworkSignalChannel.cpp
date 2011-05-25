#include <QPoint>
#include <QCursor>
#include <QXmlStreamReader>

#include <QDebug>

#include "NetworkSignalChannel.h"
#include "../storage/BehavioralDataUnitPackage.h"

namespace Picto {

NetworkSignalChannel::NetworkSignalChannel(CommandChannel *serverChannel)
	: SignalChannel(0),
	  serverChannel_(serverChannel),
	  lastTimeDataCollected_(0)
{
	//add our subchannels to the list
	addSubchannel("xpos");
	addSubchannel("ypos");
	addSubchannel("time");
}

bool NetworkSignalChannel::start()
{
	return serverChannel_->isConnected();
}

bool NetworkSignalChannel::stop()
{
	return true;
}

//!	\brief Grabs the most recent data from the server
void NetworkSignalChannel::updateDataBuffer()
{
	//Collect the data from the server
	QString commandStr = QString("GETDATA BehavioralDataUnitPackage:%1 PICTO/1.0").arg(lastTimeDataCollected_);
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(commandStr));
	QSharedPointer<Picto::ProtocolResponse> response;

	serverChannel_->sendCommand(command);
	//No response
	if(!serverChannel_->waitForResponse(1000))
		return;

	response = serverChannel_->getResponse();

	//Response not 200:OK
	if(response->getResponseCode() != Picto::ProtocolResponseType::OK)
		return;
	
	QByteArray xmlFragment = response->getContent();
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));

	while(xmlReader->readNext() && xmlReader->name() != "Data");

	Q_ASSERT(!xmlReader->atEnd());

	BehavioralDataUnitPackage behavioralData;

	xmlReader->readNext();
	while(!xmlReader->isEndElement() && xmlReader->name() != "Data" && !xmlReader->atEnd())
	{
		if(xmlReader->name() == "BehavioralDataUnitPackage")
		{
			behavioralData.fromXml(xmlReader);
		}
		else
		{
			Q_ASSERT(false);
		}
	}

	while(behavioralData.length() > 0)
	{
		// Push the data into our signal channel
		QSharedPointer<Picto::BehavioralDataUnit> dataPoint;
		dataPoint = behavioralData.takeFirstDataPoint();
		
		rawDataBuffer_["xpos"].append(dataPoint->x);
		rawDataBuffer_["ypos"].append(dataPoint->y);
		rawDataBuffer_["time"].append(dataPoint->t);

		lastTimeDataCollected_ = dataPoint->t;
	}

}


};