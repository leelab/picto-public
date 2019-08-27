#include <QXmlStreamWriter>
#include <QProcess>
#include <QBuffer>
#include <QDataStream>
#include <QDebug>
#include <QtGlobal>

#include "omniplexplugin.h"

#include "../../common/memleakdetect.h"
#include <ctime>



/*! \brief Constructs a OmniPlexPlugin object.*/
OmniPlexPlugin::OmniPlexPlugin()
{
	initialized_ = false;
	OmniPlexBuffer_ = NULL;
	int error = OPX_InitClient();
	if (error == 0)
	{
		deviceStatus_ = started;
		qDebug(QString("Created OmniPlex client").toLatin1());
	}
	else
	{
		qDebug(QString("OmniPlex InitClient error %1").arg(error).toLatin1());
		deviceStatus_ = notStarted;
	}
}

OmniPlexPlugin::~OmniPlexPlugin()
{
	OPX_CloseClient();
}

QString OmniPlexPlugin::device() const
{
	return "OmniPlex";
}


void OmniPlexPlugin::deviceSelected()
{
}

// See the Plexon OmniPlex native API example client FullClient.cpp for examples followed in using the OPX_ calls.
// 
NeuralDataAcqInterface::deviceStatus OmniPlexPlugin::startDevice()
{
	deviceStatus_ = notStarted;
	int error = -1;
	OPX_GlobalParams OPXGlobals;

	error = OPX_GetGlobalParameters(&OPXGlobals);
	Q_ASSERT(error == 0);
	if (OPXGlobals.OPXSystemType != OPXSYSTEM_DIGIAMP)
	{
		qDebug(QString("This is not an OmniPlex MiniDigi system! (type == %1)").arg(OPXGlobals.OPXSystemType).toLatin1());  // Warning only
	}
	deviceStatus_ = started;
	return deviceStatus_;
}

NeuralDataAcqInterface::deviceStatus OmniPlexPlugin::stopDevice()
{
	deviceStatus_ = notStarted;
	return deviceStatus_;
}

NeuralDataAcqInterface::deviceStatus OmniPlexPlugin::getDeviceStatus()
{
	int status = OPX_GetOPXSystemStatus();
	if(deviceStatus_ > notStarted && OmniPlexBuffer_ != NULL)
	{
		if (status < OPX_DAQ_STARTED)
		{
			deviceStatus_ = noData;
		}
		else
		{
			deviceStatus_ = hasData;
		}
	}
	return deviceStatus_;
}


float OmniPlexPlugin::samplingRate()
{
	if (SpikeResolution_ != 0)
		return (float)(1.0 / SpikeResolution_);
	else
		return 0;
}

QList<QSharedPointer<Picto::DataUnit>> OmniPlexPlugin::dumpData()
{
	QList<QSharedPointer<Picto::DataUnit>> returnList;
	QSharedPointer<Picto::NeuralDataUnit> neuralData;
	QSharedPointer<Picto::AlignmentDataUnit> alignData;
	QSharedPointer<Picto::LFPDataUnitPackage> lfpData;

	if (deviceStatus_ < started)
	{
		if (initialized_)
		{
			qDebug(QString("initialzed_ but deviceStatus_=%1").arg(deviceStatus_).toLatin1());
		}
		return returnList;
	}
	if (!initialized_)
	{
		OPX_GlobalParams OPXGlobals;

		int error = OPX_GetGlobalParameters(&OPXGlobals);
		Q_ASSERT(error == 0);
		if (OPXGlobals.OPXSystemType != OPXSYSTEM_DIGIAMP)
		{
			qDebug(QString("This is not an OmniPlex MiniDigi system! (type == %1)").arg(OPXGlobals.OPXSystemType).toLatin1());  // Warning only
		}
		error = OPX_SetDataFormat(CHANNEL_FORMAT_SOURCE_RELATIVE);
		Q_ASSERT(error == 0);
		spikeSource_ = OPX_SourceNameToSourceNumber((char *)"SPK");
		Q_ASSERT(spikeSource_ >= 0);
		LFPSource_ = OPX_SourceNameToSourceNumber((char *)"FP");
		Q_ASSERT(LFPSource_ >= 0 && LFPSource_ != spikeSource_);
		PictoEvents_ = OPX_SourceNameToSourceNumber((char *)"Other events");
		Q_ASSERT(PictoEvents_ >= 0 && PictoEvents_ != spikeSource_ && PictoEvents_ != LFPSource_);
		double rate;
		// These are used as parameters for OPX calls and their returned values are ignored
		char sourceName[256];
		char channelName[256];
		int32_t sourceType;
		int32_t linearStartChannel;
		int32_t trodality;
		int32_t ptsPerWaveform;
		int32_t preThreshPts;
		// sources are indexed from 0
		qDebug(QString("Sources: spike=%1, LFP=%2, events=%3").arg(spikeSource_).arg(LFPSource_).arg(PictoEvents_).toLatin1());
		for (int32_t s_index = 0; s_index < OPXGlobals.numSources; s_index++)
		{
			int32_t s = OPXGlobals.sourceIds[s_index];
			if (s == spikeSource_)
			{
				error = OPX_GetSourceInfoByNumber(spikeSource_, sourceName, &sourceType, &SpikeChannels_, &linearStartChannel);
				Q_ASSERT(error == 0);
				error = OPX_GetSpikeSourceInfoByNumber(spikeSource_, sourceName, &rate, &SpikeScaleByToGetVolts_, &trodality, &ptsPerWaveform, &preThreshPts);
				qDebug(QString("SpikeScaleByToGetVolts_=%1").arg(SpikeScaleByToGetVolts_).toLatin1());
				Q_ASSERT(error == 0);
				SpikeResolution_ = 1.0 / rate;
				// channels are indexed from 1, not 0
				SpikeEnables_ = new int32_t[SpikeChannels_ + 1];
				for (int32_t c = 1; c <= SpikeChannels_; c++)
				{
					// we might in the future accommodate individual channel rates and gains, but I doubt it.
					double voltageScaler; // ignored
					error = OPX_GetSourceChanInfoByNumber(spikeSource_, c, channelName, &rate, &voltageScaler, &SpikeEnables_[c]);
//					qDebug(QString("Spike voltageScaler[%1]=%2").arg(c).arg(voltageScaler).toLatin1());
					Q_ASSERT(error == 0);
					Q_ASSERT(fabs(voltageScaler - SpikeScaleByToGetVolts_) < 1e-6);
				}
			}
			else if (s == LFPSource_)
			{
				error = OPX_GetSourceInfoByNumber(LFPSource_, sourceName, &sourceType, &LFPChannels_, &linearStartChannel);
				Q_ASSERT(error == 0);
				error = OPX_GetContSourceInfoByNumber(LFPSource_, sourceName, &rate, &LFPScaleByToGetVolts_);
				qDebug(QString("LFPScaleByToGetVolts_=%1").arg(LFPScaleByToGetVolts_).toLatin1());
				Q_ASSERT(error == 0);
				LFPResolution_ = 1.0 / rate;
				// channels are indexed from 1, not 0
				LFPEnables_ = new int32_t[LFPChannels_ + 1];
				for (int32_t c = 1; c <= LFPChannels_; c++)
				{
					// we might in the future accommodate individual channel rates and gains, but I doubt it.
					double voltageScaler; // ignored
					error = OPX_GetSourceChanInfoByNumber(spikeSource_, c, channelName, &rate, &voltageScaler, &LFPEnables_[c]);
//					qDebug(QString("LFP voltageScaler[%1]=%2").arg(c).arg(voltageScaler).toLatin1());
					Q_ASSERT(error == 0);
					Q_ASSERT(fabs(voltageScaler - LFPScaleByToGetVolts_) < 1e-6);
				}
			}
			else if (s == PictoEvents_)
			{
				// The strobed input words (Picto Timing Codes) come in over channel 1 of this source
				strobeChannel_ = 1;
			}
			else
			{
				qDebug(QString("Excluding source %1").arg(s).toLatin1());
				error = OPX_ExcludeSourceByNumber(s);
				Q_ASSERT(error == 0);
			}
		}
		OmniPlexBuffer_ = (OPX_DataBlock*)malloc(sizeof(OPX_DataBlock)*MAX_OMNIPLEX_DATA_BLOCKS_PER_READ);
		qDebug(QString("Created OmniPlexBuffer at 0x%1 size %2 bytes").arg((quintptr)OmniPlexBuffer_, QT_POINTER_SIZE * 2, 16, QChar('0')).arg(sizeof(OPX_DataBlock)*MAX_OMNIPLEX_DATA_BLOCKS_PER_READ).toLatin1());
		initialized_ = true;
	}
	int32_t blocksRead = MAX_OMNIPLEX_DATA_BLOCKS_PER_READ;
	int error = OPX_GetNewData(&blocksRead, OmniPlexBuffer_);
	Q_ASSERT(error == 0);
	if (blocksRead == MAX_OMNIPLEX_DATA_BLOCKS_PER_READ)
	{
		qDebug(QString("OmniPlex data acquisition saturated").toLatin1());  // Warning
	}
	for (int block = 0; block < blocksRead; block++)
	{
		Q_ASSERT(OmniPlexBuffer_[block].NumberOfDataWords <= MAX_WF_LENGTH);
		int32_t srcNum = OmniPlexBuffer_[block].SourceNumOrType;
		int32_t channel = OmniPlexBuffer_[block].Channel;
		int32_t unit = OmniPlexBuffer_[block].Unit;
		if (srcNum == spikeSource_ && SpikeEnables_[channel] != 0 && unit != 0)  // Unit 0 is all spikes and unwanted
		{
			neuralData = QSharedPointer<Picto::NeuralDataUnit>(new Picto::NeuralDataUnit());
			double timestampSec = OPX_Get64BitDoubleTimestampFromDataBlock(&OmniPlexBuffer_[block]);
			neuralData->setTimestamp(timestampSec);
			neuralData->setChannel(channel);
			neuralData->setResolution(SpikeResolution_);
			neuralData->setUnit(unit);
			QSharedPointer<QVector<float>> waveform(new QVector<float>);
			for (int i = 0; i<OmniPlexBuffer_[block].NumberOfDataWords; i++)
			{
				waveform->push_back(double(OmniPlexBuffer_[block].WaveForm[i]) * SpikeScaleByToGetVolts_);
			}
			neuralData->setWaveform(waveform);
			returnList.push_back(neuralData);
		}
		if (srcNum == LFPSource_ && LFPEnables_[channel] != 0)
		{
			double timestampSec = OPX_Get64BitDoubleTimestampFromDataBlock(&OmniPlexBuffer_[block]);
			int lfpChan = channel;

			// If lfpData_ vector is too small, make it bigger
			if (lfpData_.size() < lfpChan + 1)
				lfpData_.resize(lfpChan + 1);
			for (int i = 0; i<OmniPlexBuffer_[block].NumberOfDataWords; i++)
			{
				// If the current channel's lfpDataUnit is big enough.  (ie. on the next entry it will include over 500 ms of data)
				// Add it to the list.
				if (!lfpData_[lfpChan].isNull()
					&& (lfpData_[lfpChan]->numSamples()*lfpData_[lfpChan]->getResolution()) >= 0.5)
				{
					returnList.push_back(lfpData_[lfpChan]);
					lfpData_[lfpChan].clear();
				}
				// If this channel's lfpDataUnit hasn't been initialized, initialize it.
				if (lfpData_[lfpChan].isNull())
				{
					lfpData_[lfpChan] = QSharedPointer<Picto::LFPDataUnitPackage>(new Picto::LFPDataUnitPackage());
					lfpData_[lfpChan]->setChannel(lfpChan);
					lfpData_[lfpChan]->setResolution(LFPResolution_);
					lfpData_[lfpChan]->setTimestamp(timestampSec + (LFPResolution_*i));
				}
				lfpData_[lfpChan]->appendData(double(OmniPlexBuffer_[block].WaveForm[i]) * LFPScaleByToGetVolts_);
			}
		}
		if (srcNum == PictoEvents_ && channel == strobeChannel_)
		{
			// alignment codes
			alignData = QSharedPointer<Picto::AlignmentDataUnit>(new Picto::AlignmentDataUnit());
			double timestampSec = OPX_Get64BitDoubleTimestampFromDataBlock(&OmniPlexBuffer_[block]);
			alignData->setTimestamp(timestampSec);
			// We mask the align code with 0x007F because we only care about the 
			// bottom 7 lines since Picto's event codes are always 0x7f and lower.
			// Anything that accidentally came in on another line should be ignored.
			alignData->setAlignCode((OmniPlexBuffer_[block].Unit & 0x007F));
			returnList.push_back(alignData);
			// Code to output 1 channel of LFP values, before sending them to picto, for comparison with OmniPlex recorded values
			// qDebug(QString("timing code=%1").arg(OmniPlexBuffer_[block].Unit & 0x007F).toLatin1());
		}
	}
	return returnList;
}

bool OmniPlexPlugin::acqDataAfterNow()
{
	int error = OPX_ClearData(0); // toss backlogged data
	return (error == 0);
}

//Q_EXPORT_PLUGIN2(ProxyPluginOmniPlex, OmniPlexPlugin)