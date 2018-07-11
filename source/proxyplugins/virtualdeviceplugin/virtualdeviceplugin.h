#ifndef VIRTUALDEVICEPLUGIN_H
#define VIRTUALDEVICEPLUGIN_H


#include <QObject>
#include <QStringList>
#include <QSharedPointer>
#include <QVector>

#include "../../proxyserver/NeuralDataAcqInterface.h"
#include "../../common/timing/Timestamper.h"
#include "VirtualEventSource.h"
#include "QProgressBar.h"

#define MAX_MAP_EVENTS_PER_READ 5000

/*! \brief A plugin allowing the proxy server to interface with a VirtualDevice
 *
 *	\details This plugin runs a virtual device and returns its alignment events, spikes and LFP readings
 *  through the standard proxy plugin interface.  It is purely for debugging 
 *  purposes at this point and it can only be extended to run a different
 *  type of virtual device from within the C++ code itself.  The plugin relies on three types of 
 *	VirtualEventSource objects, SimpleSpikeSource, SimpleMarkSource, and SimpleLFPSource.  
 *	Every time dumpData is called, the plugin calls getNextEvent() on each data source over and over until
 *  getNextEvent() returns a null event.  The VirtualEventSource objects always return data when getNextEvent() is
 *	called until all of their virtual data up to the current time has been used up.  This way, dumpData() can 
 *	be sure that it has all available virtual data when getNextEvent() returns an empty pointer for all VirtualEventSources.
 *	After all data from VirtualEventSources is loaded, the data is added to the message that is sent to the 
 *	Picto Server by the Proxy application that calls dumpData().
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class VirtualDevicePlugin : public QObject, public NeuralDataAcqInterface
{
    Q_OBJECT
    Q_INTERFACES(NeuralDataAcqInterface)
	Q_PLUGIN_METADATA(IID "org.blockdesign.Picto.ProxyServer.NeuralDataAcqInterface/1.0")

public:
	VirtualDevicePlugin();
    QString device() const;
	NeuralDataAcqInterface::deviceStatus startDevice();
	NeuralDataAcqInterface::deviceStatus stopDevice();
	NeuralDataAcqInterface::deviceStatus getDeviceStatus();
	float samplingRate();
	QList<QSharedPointer<Picto::DataUnit>> dumpData();
	bool acqDataAfterNow();
	void alignSysTimes(QString rhdFile, QString saveFolder, QProgressBar* bar, QString sqlFile, QString mdaFile, bool convert = true){};

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