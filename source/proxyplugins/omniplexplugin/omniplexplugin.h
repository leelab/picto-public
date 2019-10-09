#ifndef OMNIPLEXPLUGIN_H
#define OMNIPLEXPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QSharedMemory>
#define _AMD64_ 
typedef unsigned short      WORD;
typedef unsigned long  DWORD;
#define WINAPI      __stdcall
#include <windef.h>
#include <winnt.h>
#include <stdint.h>
#include <windows.h>
#include "OPXClient.h"

#include "../../proxyserver/NeuralDataAcqInterface.h"
#include "QProgressBar.h"

#define MAX_OMNIPLEX_DATA_BLOCKS_PER_READ 128000

/*! \brief A plugin allowing the proxy server to interface with a Plexon OmniPlex
 *
 *	\details This uses the OmniPlex native API provided by Plexon to extract neural data from a running Plexon 
 *	system.  All of the functions do what they are supposed to do according to the NeuralDataAcqInterface 
 *  documentation.  This class simply implements those functions for the OmniPlex interface.
 *	\note The OPXClient64.dll must be in the path for this plugin to be loaded.  Currently we place
 *	it in the bin directory alongside the proxy plugin executable.
 *	\author Mike Scudder, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2019
 */
class OmniPlexPlugin : public QObject, public NeuralDataAcqInterface
{
    Q_OBJECT
    Q_INTERFACES(NeuralDataAcqInterface)
	Q_PLUGIN_METADATA(IID "org.blockdesign.Picto.ProxyServer.NeuralDataAcqInterface/1.0")

public:
	OmniPlexPlugin();
	virtual ~OmniPlexPlugin();
    QString device() const;
	void deviceSelected();
	NeuralDataAcqInterface::deviceStatus startDevice();
	NeuralDataAcqInterface::deviceStatus stopDevice();
	NeuralDataAcqInterface::deviceStatus getDeviceStatus();
	float samplingRate();
	QList<QSharedPointer<Picto::DataUnit>> dumpData();
	bool acqDataAfterNow();
private:
	NeuralDataAcqInterface::deviceStatus deviceStatus_;
	int32_t spikeSource_;
	int32_t LFPSource_;
	int32_t PictoEvents_;
	int32_t strobeChannel_;
	int LFPChannels_;
	int SpikeChannels_;
	// These are assumed to be the same for all channels
	double LFPScaleByToGetVolts_;
	double SpikeScaleByToGetVolts_;
	double LFPResolution_;
	double SpikeResolution_;
	// These can be set specifically per channel
	int32_t *LFPEnables_;
	int32_t *SpikeEnables_;
	bool foundOmniPlex_;
	// data buffers
	bool initialized_;
	OPX_DataBlock *OmniPlexBuffer_;
	QVector<QSharedPointer<Picto::LFPDataUnitPackage>> lfpData_;
protected:
};

#endif