#ifndef INTERFACES_H
#define INTERFACES_H

#include <QtPlugin>
#include <QSharedPointer>
#include "../common/storage/NeuralDataUnit.h"
#include "../common/storage/AlignmentDataUnit.h"
#include "../common/storage/LFPDataUnitPackage.h"

/*! \brief Interface used for all proxy server plugins
 *
 *	This defines the interface used by proxy server plugins.  If you want to build a new
 *	plugin (for a different device), you'll need to make sure that it has these functions.
 */
class NeuralDataAcqInterface
{
public:
	enum deviceStatus
	{
		stopped,
		started,
		running,
		failedToStart
	};

    virtual ~NeuralDataAcqInterface() {}

    virtual QString device() const = 0;
	virtual void deviceSelected(){};
	virtual deviceStatus startDevice() = 0;
	virtual deviceStatus stopDevice() = 0;
	virtual deviceStatus getDeviceStatus() = 0;
	virtual float samplingRate() = 0;
	virtual QList<QSharedPointer<Picto::DataUnit>> dumpData() = 0;
};

QT_BEGIN_NAMESPACE

Q_DECLARE_INTERFACE(NeuralDataAcqInterface,
                    "org.blockdesign.Picto.ProxyServer.NeuralDataAcqInterface/1.0")

QT_END_NAMESPACE



#endif