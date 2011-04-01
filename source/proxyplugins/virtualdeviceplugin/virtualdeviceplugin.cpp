#include <QXmlStreamWriter>
#include <windows.h>
#include "virtualdeviceplugin.h"
#include "simplespikesource.h"
#include "simplemarksource.h"
#include "simplelfpsource.h"

QSharedPointer<Picto::Timestamper> VirtualDevicePlugin::timeStamper_ = QSharedPointer<Picto::Timestamper>(new Picto::Timestamper());

/*!	\brief	Used to define the VirtualEventSources to which this VirtualDevicePlugin is attached.
 *
 *	To use a different type of VirtualEventSource, include it above and replace the two 
 *  constructors in this function.  We could also use a dialog, but that is overkill
 *  right now.
 */
void VirtualDevicePlugin::CreateEventSources()
{
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(5.0, 0.001)));
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleMarkSource(5.0, 0.001)));
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(5.0, 0.001)));
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
	latestEvents_.clear();
	bool started = true;
	foreach(QSharedPointer<VirtualEventSource> source,sources_)
	{
		if(!source->start(currTime))
		{
			started = false;
			break;
		}
	}
	if(started)
	{
		status_ = running;
	}
	else
	{
		foreach(QSharedPointer<VirtualEventSource> source,sources_)
		{
			source->stop();
		}
		status_ = failedToStart;
	}
	return status_;
}

NeuralDataAcqInterface::deviceStatus VirtualDevicePlugin::stopDevice()
{
	foreach(QSharedPointer<VirtualEventSource> source,sources_)
	{
		source->stop();
	}
	return stopped;
}

NeuralDataAcqInterface::deviceStatus VirtualDevicePlugin::getDeviceStatus()
{
	return status_;
}


float VirtualDevicePlugin::samplingRate()
{
	float rate = -1;
	foreach(QSharedPointer<VirtualEventSource> source,sources_)
	{
		if((source->samplingRate() < rate) || (rate == -1))
			rate = source->samplingRate();
	}
	return rate;
}

/*!	\brief	Gets new events from spikeSource and markSource until they return NULL and returns them.
 */
QSharedPointer<Picto::DataStore> VirtualDevicePlugin::getNextEvent(double currTime)
{
	if(latestEvents_.size())
	{
		return latestEvents_.takeFirst();
	}
	QSharedPointer<Picto::DataStore> latestEvent;
	foreach(QSharedPointer<VirtualEventSource> source,sources_)
	{
		latestEvent = source->getNextEvent(currTime);
		if(latestEvent.isNull())
			continue;
		latestEvents_.push_back(latestEvent);
	}
	if(latestEvents_.size())
	{
		return latestEvents_.takeFirst();
	}
	return QSharedPointer<Picto::DataStore>();
}

QList<QSharedPointer<Picto::DataStore>> VirtualDevicePlugin::dumpData()
{
	dataList_.clear();
	updateData();
	return dataList_;
}

/*!	\brief	Calls getNextEvent() over and over, adding new events to the dataStores until it gets null back.
 */
void VirtualDevicePlugin::updateData()
{
	double currTime = timeStamper_->stampSec();
	QSharedPointer<Picto::DataStore> currEvent = getNextEvent(currTime);
	while(!currEvent.isNull())
	{
		dataList_.push_back(currEvent);
		currEvent = getNextEvent(currTime);
	}
}

Q_EXPORT_PLUGIN2(ProxyPluginVirtualDevice, VirtualDevicePlugin)