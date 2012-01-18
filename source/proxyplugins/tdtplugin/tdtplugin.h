#ifndef TDTPLUGIN_H
#define TDTPLUGIN_H


#include <QObject>
#include <QVector>
#include <QMap>
#include <QMutex>

//#include "../../proxyserver/interfaces.h"	// Joey - 12/27/10 This file no longer exists, replaced with the following
#include "../../proxyserver/NeuralDataAcqInterface.h"
#import "C:\\TDT\\OpenEx\\OCX\\TTankX.ocx"

/*! \brief A plugin allowing the proxy server to interface with a TDT system
 *
 *	This uses the Microsoft COM interface to talk to the TDT system.  This interface is
 *	well-documented by TDT.
 */

class TdtPlugin : public QObject, public NeuralDataAcqInterface
{
    Q_OBJECT
    Q_INTERFACES(NeuralDataAcqInterface)

public:
	TdtPlugin();
    QString device() const;
	virtual void deviceSelected();
	NeuralDataAcqInterface::deviceStatus startDevice();
	NeuralDataAcqInterface::deviceStatus stopDevice();
	NeuralDataAcqInterface::deviceStatus getDeviceStatus();
	float samplingRate();
	QList<QSharedPointer<Picto::DataUnit>> dumpData();

private:
	struct SpikeDetails
	{
		QVector<double> sampleWaveform;
		int chanNum;
		int unitNum;
		double timeStamp;
	};
	struct EventDetails
	{
		int code;
		double timeStamp;
	};

	bool startCOM();
	void stopCOM();
	static bool spikeTimestampLessThan(const SpikeDetails &sd1, const SpikeDetails &sd2);
	static bool eventTimestampLessThan(const EventDetails &ed1, const EventDetails &ed2);


	TTANKXLib::_DTTankXPtr tdtTank;
	float sampleRate;
	double lastTimestamp;
	bool bDeviceRunning;

	QMutex COMMutex;


	

	wchar_t *szServerName;
	wchar_t *szTankName;
	wchar_t *szBlockName;
	QVector<QSharedPointer<Picto::LFPDataUnitPackage>> lfpData_;



};


#endif