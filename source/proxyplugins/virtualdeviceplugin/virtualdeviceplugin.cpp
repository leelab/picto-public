#include <QXmlStreamWriter>
#include <windows.h>
#include "virtualdeviceplugin.h"
#include "simplespikesource.h"
#include "simplemarksource.h"
#include "simplelfpsource.h"
#include "../../common/memleakdetect.h"

QSharedPointer<Picto::Timestamper> VirtualDevicePlugin::timeStamper_ = QSharedPointer<Picto::Timestamper>(new Picto::Timestamper());

/*!	\brief	Used to define the VirtualEventSources to which this VirtualDevicePlugin is attached.
 *
 *	To use a different type of VirtualEventSource, include it above and replace the two 
 *  constructors in this function.  We could also use a dialog, but that is overkill
 *  right now.
 */
void VirtualDevicePlugin::CreateEventSources()
{
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleMarkSource(6.0, 0.001)));

	//Heavy Insane Recording------------------------------------------------------------------------
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,0,0)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,1,0)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,2,0)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,3,0)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,4,0)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,5,0)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,6,0)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,7,0)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,8,0)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,9,0)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,0,1)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,1,1)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,2,1)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,3,1)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,4,1)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,5,1)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,6,1)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,7,1)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,8,1)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.01,9,1)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,0)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,1)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,2)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,3)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,4)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,5)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,6)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,7)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,8)));
	//sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,9)));
	//-----------------------------------------------------------------------------------------------

	//Heavy Standard Recording--------------------------------------------------------------------
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.02, 0.001,1,0)));
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.01, 0.001,1,1)));
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.06, 0.001,3,0)));
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleSpikeSource(.08, 0.001,4,0)));
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,1)));
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,2)));
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,3)));
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,4)));
	sources_.push_back(QSharedPointer<VirtualEventSource>(new SimpleLFPSource(.5, 0.001,5)));
	//--------------------------------------------------------------------------------------------
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
QSharedPointer<Picto::DataUnit> VirtualDevicePlugin::getNextEvent(double currTime)
{
	if(latestEvents_.size())
	{
		return latestEvents_.takeFirst();
	}
	QSharedPointer<Picto::DataUnit> latestEvent;
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
	return QSharedPointer<Picto::DataUnit>();
}

QList<QSharedPointer<Picto::DataUnit>> VirtualDevicePlugin::dumpData()
{
	dataList_.clear();
	updateData();
	return dataList_;
}

/*!	\brief	Calls getNextEvent() over and over, adding new events to the DataUnits until it gets null back.
 */
void VirtualDevicePlugin::updateData()
{
	double currTime = timeStamper_->stampSec();
	QSharedPointer<Picto::DataUnit> currEvent = getNextEvent(currTime);
	while(!currEvent.isNull())
	{
		dataList_.push_back(currEvent);
		currEvent = getNextEvent(currTime);
	}
}

Q_EXPORT_PLUGIN2(ProxyPluginVirtualDevice, VirtualDevicePlugin)