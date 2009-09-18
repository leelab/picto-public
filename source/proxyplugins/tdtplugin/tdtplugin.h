#ifndef PLEXONPLUGIN_H
#define PLEXONPLUGIN_H


#include <QObject>
#include <QStringList>

#include "../../proxyserver/interfaces.h"

class TdtPlugin : public QObject, public NeuralDataAcqInterface
{
    Q_OBJECT
    Q_INTERFACES(NeuralDataAcqInterface)

public:
    QString device() const;
	NeuralDataAcqInterface::deviceStatus startDevice();
	NeuralDataAcqInterface::deviceStatus stopDevice();
	NeuralDataAcqInterface::deviceStatus getDeviceStatus();
	float samplingRate();
	QString dumpData();
};


#endif