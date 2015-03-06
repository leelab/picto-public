#ifndef TDTPLUGIN_H
#define TDTPLUGIN_H


#include <QObject>
#include <QVector>
#include <QMap>
#include <QMutex>

//#include "../../proxyserver/interfaces.h"	// Joey - 12/27/10 This file no longer exists, replaced with the following
#include "../../proxyserver/NeuralDataAcqInterface.h"
#import "TTankX.ocx"

/*! \brief A plugin allowing the proxy server to interface with a TDT system
 *
 *	\details This uses the Microsoft COM interface to talk to the TDT system.  This interface is
 *	well-documented by TDT. All of the functions do what they are supposed to do according 
 *	to the NeuralDataAcqInterface documentation.  This class simply implements those 
 *	functions for the TDT interface.  Some additional functions were created purely for
 *	the purpose of cleaning up the inherited functions.  Those functions have been documented here
 *	but they are really just another part of the TDT interface logic.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class TdtPlugin : public QObject, public NeuralDataAcqInterface
{
    Q_OBJECT
    Q_INTERFACES(NeuralDataAcqInterface)
	Q_PLUGIN_METADATA(IID "org.blockdesign.Picto.ProxyServer.NeuralDataAcqInterface/1.0")

public:
	TdtPlugin();
    QString device() const;
	virtual void deviceSelected(){};
	NeuralDataAcqInterface::deviceStatus startDevice();
	NeuralDataAcqInterface::deviceStatus stopDevice();
	NeuralDataAcqInterface::deviceStatus getDeviceStatus();
	float samplingRate();
	QList<QSharedPointer<Picto::DataUnit>> dumpData();
	bool acqDataAfterNow();

private:

	/*! \brief A simple struct used for organizing Spike data.*/
	struct SpikeDetails
	{
		QVector<float> sampleWaveform;
		int chanNum;
		int unitNum;
		double timeStamp;
		double resolution;
	};

	/*! \brief A simple struct used for organizing Event code data.
	 *	\note Event codes are actually alignment codes that were sent in from the Picto Director.
	 */
	struct EventDetails
	{
		int code;
		double timeStamp;
	};

	bool getRunningBlockInfo();
	bool startCOM();
	void stopCOM();
	static bool spikeTimestampLessThan(const SpikeDetails &sd1, const SpikeDetails &sd2);
	static bool eventTimestampLessThan(const EventDetails &ed1, const EventDetails &ed2);


	TTANKXLib::_DTTankXPtr tdtTank;
	float sampleRate;
	float samplePeriod;
	double lastEventTimestamp_;
	double lastSpikeTimestamp_;
	double lastLFPTimestamp_;


	

	wchar_t *szServerName;
	wchar_t *szTankName;
	wchar_t *szBlockName;
	/*! \brief A simple struct used for organizing LFPData.*/
	struct LFPData
	{
		LFPData(){currTime = -1;};
		double currTime;
		QSharedPointer<Picto::LFPDataUnitPackage> lfpPackage;
	};
	QVector<LFPData> lfpData_;

	deviceStatus deviceStatus_;



};


#endif