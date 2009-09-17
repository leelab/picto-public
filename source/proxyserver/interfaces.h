/*! \file interfaces.h
 * \ingroup pictoproxyserver
 * \brief Interface used for all proxy server plugins
 */

#ifndef INTERFACES_H
#define INTERFACES_H

#include <QtPlugin>

class NeuralDataAcqInterface
{
public:
    virtual ~NeuralDataAcqInterface() {}

    virtual QString device() const = 0;
	virtual QString deviceName() = 0;
	virtual QString deviceStatus() = 0;
	virtual float samplingRate() = 0;
	virtual QString dumpData() = 0;


	
};

QT_BEGIN_NAMESPACE

Q_DECLARE_INTERFACE(NeuralDataAcqInterface,
                    "org.blockdesign.Picto.ProxyServer.NeuralDataAcqInterface/1.0")

QT_END_NAMESPACE



#endif