#ifndef TDTPLUGIN_H
#define TDTPLUGIN_H


#include <QObject>
#include <QVector>
#include <QMap>

#include "../../proxyserver/interfaces.h"
#import "C:\\TDT\\OpenEx\\OCX\\TTankX.ocx"


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

private:
	bool startCOM();
	void stopCOM();

	TTANKXLib::_DTTankXPtr tdtTank;
	float sampleRate;
	double lastTimestamp;

	struct SpikeDetails
	{
		QVector<double> sampleWaveform;
		int chanNum;
		int unitNum;
		double timeStamp;
	};

	QVector<SpikeDetails> spikeList;

	struct EventDetails
	{
		int code;
		double timeStamp;
	};

	QVector<EventDetails> eventList;
	
	static bool spikeTimestampLessThan(const SpikeDetails &sd1, const SpikeDetails &sd2);
	static bool eventTimestampLessThan(const EventDetails &ed1, const EventDetails &ed2);

	wchar_t *szServerName;
	wchar_t *szTankName;
	wchar_t *szBlockName;



};


#endif