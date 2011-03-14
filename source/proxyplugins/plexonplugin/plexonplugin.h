#ifndef PLEXONPLUGIN_H
#define PLEXONPLUGIN_H


#include <QObject>
#include <QStringList>

#include "../../proxyserver/NeuralDataAcqInterface.h"

#define MAX_MAP_EVENTS_PER_READ 5000

/*! \brief A plugin allowing the proxy server to interface with a Plexon MAP
 *
 *	This uses the API provided by Plexon to extract neural data from a running Plexon 
 *	system.  The documentation for this APi is relatively easy to come by. Unfortunately
 *	I made the mistake of designing these plugins to return raw XML.  In a refactoring, 
 *	it would be wise to 
 */

class PlexonPlugin : public QObject, public NeuralDataAcqInterface
{
    Q_OBJECT
    Q_INTERFACES(NeuralDataAcqInterface)

public:
    QString device() const;
	NeuralDataAcqInterface::deviceStatus startDevice();
	NeuralDataAcqInterface::deviceStatus stopDevice();
	NeuralDataAcqInterface::deviceStatus getDeviceStatus();
	float samplingRate();
	QList<QSharedPointer<Picto::DataStore>> dumpData();
};


#endif