#include <QXmlStreamWriter>
#include <windows.h>
#include "virtualdeviceplugin.h"
#include "simplespikesource.h"
#include "simplemarksource.h"

QSharedPointer<Picto::Timestamper> VirtualDevicePlugin::timeStamper_ = QSharedPointer<Picto::Timestamper>(new Picto::Timestamper());
QSharedPointer<VirtualEventSource> VirtualDevicePlugin::spikeSource_ = QSharedPointer<VirtualEventSource>();
QSharedPointer<VirtualEventSource> VirtualDevicePlugin::markSource_ = QSharedPointer<VirtualEventSource>();

/*!	\brief	Used to define the VirtualEventSources to which this VirtualDevicePlugin is attached.
 *
 *	To use a different type of VirtualEventSource, include it above and replace the two 
 *  constructors in this function.  We could also use a dialog, but that is overkill
 *  right now.
 */
void VirtualDevicePlugin::CreateEventSources()
{
	if(spikeSource_.isNull())
		spikeSource_ = QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(1.0, 1.0));
	if(markSource_.isNull())
		markSource_ = QSharedPointer<VirtualEventSource>(new SimpleMarkSource(1.0, 1.0));
}

QString VirtualDevicePlugin::device() const
{
	return "VirtualDevice";
}

NeuralDataAcqInterface::deviceStatus VirtualDevicePlugin::startDevice()
{
	double currTime = timeStamper_->stampSec();
	lastEvent_ = QSharedPointer<VirtualEvent>();
	CreateEventSources();
	if(spikeSource_->start(currTime) && markSource_->start(currTime))
	{
		status_ = running;
	}
	else
	{
		spikeSource_->stop();
		markSource_->stop();
		status_ = failedToStart;
	}
	return status_;
}

NeuralDataAcqInterface::deviceStatus VirtualDevicePlugin::stopDevice()
{
	spikeSource_->stop();
	markSource_->stop();
	return stopped;
}

NeuralDataAcqInterface::deviceStatus VirtualDevicePlugin::getDeviceStatus()
{
	return status_;
}


float VirtualDevicePlugin::samplingRate()
{
	if(spikeSource_->samplingRate() <= markSource_->samplingRate())
		return spikeSource_->samplingRate();
	else 
		return markSource_->samplingRate();
}

/*!	\brief	Calls getNextEvent() over and over, adding new events to the xml until it gets null back.
 */
QString VirtualDevicePlugin::dumpData()
{
	QString xmlData;
	QXmlStreamWriter writer(&xmlData);
	writer.setAutoFormatting(true);
	double currTime = timeStamper_->stampSec();

	unsigned int numEvents = 0;

	QSharedPointer<VirtualEvent> currEvent = getNextEvent(currTime);
	while(!currEvent.isNull())
	{
		numEvents++;
		if(currEvent->type_ == VirtualEvent::eSPIKE)
		{
			//spike events
			writer.writeStartElement("event");
			writer.writeAttribute("type", "spike");

			writer.writeTextElement("timestamp",QString("%1").arg(currEvent->timeStamp_,0,'f',4));

			writer.writeTextElement("channel",QString("%1").arg(currEvent->channel_));

			char chUnit = 'a'+currEvent->unit_-1;
			writer.writeTextElement("unit",QString("%1").arg(chUnit));

			//waveform data
			writer.writeStartElement("wave");
			for(int i=0; i<currEvent->waveform_.size(); i++)
			{
				writer.writeCharacters(QString("%1 ").arg(currEvent->waveform_[i]));
			}
			writer.writeEndElement();			

			writer.writeEndElement(); //end event
		}
		else if(currEvent->type_ == VirtualEvent::eMARK)
		{
			// eventcodes
			writer.writeStartElement("event");
			writer.writeAttribute("type","externalEvent");

			writer.writeTextElement("timestamp",QString("%1").arg(currEvent->timeStamp_,0,'f',4));

			writer.writeTextElement("eventCode",QString("%1").arg(currEvent->eventCode_));

			writer.writeEndElement(); //event
		}
		currEvent = getNextEvent(currTime);
	}
	//number of events
	writer.writeStartElement("numEvents");
	writer.writeCharacters(QString("%1").arg(numEvents));
	writer.writeEndElement();
	return xmlData;
}

/*!	\brief	Gets new events from spikeSource and markSource until they return NULL and returns them.
 *
 *	This function also sorts the spikes and mark events into increasing timestamp order.
 */
QSharedPointer<VirtualEvent> VirtualDevicePlugin::getNextEvent(double currTime)
{
	if(!lastEvent_.isNull())
	{	
		// This is not the first of the calls for this timeframe
		switch(lastEvent_->type_)
		{
		case VirtualEvent::eSPIKE: // Last event was a spike.  Get a new one.
				lastSpike_ = spikeSource_->getNextEvent(currTime);
			break;
		case VirtualEvent::eMARK: // Last event was a mark.  Get a new one.
				lastMark_ = markSource_->getNextEvent(currTime);
			break;
		}
	}
	else
	{
		// This is the first of the calls for this timeframe
		lastSpike_ = spikeSource_->getNextEvent(currTime);
		lastMark_ = markSource_->getNextEvent(currTime);
	}

	// If either event is null, return the other.  Otherwise, choose the event that
	// happened first.  If all events are null, lastEvent_ will be null.
	if(lastMark_.isNull()) lastEvent_ = lastSpike_;
	else if(lastSpike_.isNull()) lastEvent_ = lastMark_;
	else if(lastMark_->timeStamp_ < lastSpike_->timeStamp_) lastEvent_ = lastMark_;
	else lastEvent_ = lastSpike_;
	return lastEvent_;
}

Q_EXPORT_PLUGIN2(ProxyPluginVirtualDevice, VirtualDevicePlugin)