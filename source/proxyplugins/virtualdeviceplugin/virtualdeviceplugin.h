#ifndef VIRTUALDEVICEPLUGIN_H
#define VIRTUALDEVICEPLUGIN_H


#include <QObject>
#include <QStringList>
#include <QSharedPointer>
#include <QVector>

#include "../../proxyserver/NeuralDataAcqInterface.h"
#include "../../common/timing/Timestamper.h"
#include "VirtualEventSource.h"

#define MAX_MAP_EVENTS_PER_READ 5000

/*! \brief A plugin allowing the proxy server to interface with a VirtualDevice
 *
 *	This plugin runs a virtual device and returns its marking events and spikes
 *  through the standard proxy plugin interface.  It is purely for debugging 
 *  purposes as this point so that it can only be extended to run a different
 *  type of virtual device in code.  The plugin relies on two VirtualEventSource
 *  objects, a spike source and a mark source.  Every time dumpData is called,
 *  the plugin calls getNextEvent() on each data source over and over until
 *  getNextEvent() returns a null event.  Events are sorted for increasing 
 *  timestamp automatically and added to the xml message that is sent to 
 *  the pictoserver through the plugin interface.
 */
class VirtualDevicePlugin : public QObject, public NeuralDataAcqInterface
{
    Q_OBJECT
    Q_INTERFACES(NeuralDataAcqInterface)

public:
    QString device() const;
	NeuralDataAcqInterface::deviceStatus startDevice();
	NeuralDataAcqInterface::deviceStatus stopDevice();
	NeuralDataAcqInterface::deviceStatus getDeviceStatus();
	float samplingRate();
	QList<QSharedPointer<Picto::DataUnit>> dumpData();
private:
	void updateData();
	deviceStatus status_;
	QSharedPointer<Picto::DataUnit> getNextEvent(double currTime);
	QSharedPointer<VirtualEvent> lastEvent_, lastSpike_, lastMark_;
	QList<QSharedPointer<Picto::DataUnit>> dataList_;
	QList<QSharedPointer<VirtualEventSource>> sources_;
	QList<QSharedPointer<Picto::DataUnit>> latestEvents_;
	static QSharedPointer<Picto::Timestamper> timeStamper_;
	void CreateEventSources();
};

#endif