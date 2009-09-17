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
	QString deviceName();
	QString deviceStatus();
	float samplingRate();
	QString dumpData();
};


#endif