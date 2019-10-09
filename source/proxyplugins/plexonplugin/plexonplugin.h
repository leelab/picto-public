#ifndef PLEXONPLUGIN_H
#define PLEXONPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QSharedMemory>

struct PL_WaveLong;
struct PL_Event;

QT_BEGIN_NAMESPACE
class QProcess;
QT_END_NAMESPACE

#include "../../proxyserver/NeuralDataAcqInterface.h"
#include "QProgressBar.h"

#define MAX_MAP_EVENTS_PER_READ 5000

/*! \brief A plugin allowing the proxy server to interface with a Plexon MAP
 *
 *	\details This uses the API provided by Plexon to extract neural data from a running Plexon 
 *	system.  The documentation for this API is relatively easy to come by.  All of the functions
 *	do what they are supposed to do according to the NeuralDataAcqInterface documentation.  This
 *	class simply implements those functions for the Plexon interface.
 *	\note The PlexClient.dll must be in the path for this plugin to be loaded.  Currently we place
 *	it in the PictoRunPath\\plugins directory alongside the dll generated for this class.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PlexonPlugin : public QObject, public NeuralDataAcqInterface
{
    Q_OBJECT
    Q_INTERFACES(NeuralDataAcqInterface)
	Q_PLUGIN_METADATA(IID "org.blockdesign.Picto.ProxyServer.NeuralDataAcqInterface/1.0")

public:
	PlexonPlugin();
	virtual ~PlexonPlugin();
    QString device() const;
	void deviceSelected();
	NeuralDataAcqInterface::deviceStatus startDevice();
	NeuralDataAcqInterface::deviceStatus stopDevice();
	NeuralDataAcqInterface::deviceStatus getDeviceStatus();
	float samplingRate();
	QList<QSharedPointer<Picto::DataUnit>> dumpData();
	bool acqDataAfterNow();
private:
	QVector<QSharedPointer<Picto::LFPDataUnitPackage>> lfpData_;
	NeuralDataAcqInterface::deviceStatus deviceStatus_; 
	int freq;
	int channels[128];
	int lfpGains[128];
	int spikeGains[128];

protected:
	void CheckAndOpenWrapper();

	void CloseClient();
	int InitClientEx3(int type);
	int IsSortClientRunning();
	int GetTimeStampTick();
	void GetSlowInfo(int* freq, int* channels, int* gains);
	void GetGain(int* gain);
	void GetLongWaveFormStructures(int* pnmax, PL_WaveLong* waves, int* serverdropped, int* mmfdropped);
	void GetTimeStampStructures(int* pnmax, PL_Event* events);
	void CommandQuit();

	QSharedMemory m_sharedMemory;
	QProcess *myProcess;
	bool m_bWrapperRunning;
};


#endif