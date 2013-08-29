#ifndef _LiveSpikeReader_H_
#define _LiveSpikeReader_H_
#include <QVector>
#include "../../common/playback/PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Frame PlaybackData values for use in Playback system.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API LiveSpikeReader : public SpikeReader
#else
class LiveSpikeReader : public SpikeReader
#endif
{
	Q_OBJECT
public:
	LiveSpikeReader(int maxChan,int maxUnit,int waveformSize);
	void setRunStart();
	void createVirtualSpike(double time);
	void setRunEnd();

	//Frame Reader Interface
	//Spike Reader Interface
	virtual QVariantList getChannels();
	virtual QVariantList getUnits(int channel);
	virtual double getLatestTime();
	virtual int getLatestChannel();
	virtual int getLatestUnit();
	virtual QVariantList getLatestWaveform();
	virtual double getNextTime();
	virtual int getNextChannel();
	virtual int getNextUnit();
	virtual QVariantList getNextWaveform();
	
	//Returns a list of spikes times when that occured with times > the input time and 
	//<= the current time on all channels and units.  There is a one to one matchup of times
	//from this function and channels from getChannelsSince(time), units from getUnitsSince(time)
	//and waveforms from getWaveformsSince(time).
	virtual QVariantList getTimesSince(double time);
	//Returns a list of spikes times when that occured with times > the current time and 
	// <= the input time on the all channels and units.  There is a one to one matchup of times
	//from this function and channels from getChannelsUntil(time), units from getUnitsUntil(time)
	//and waveforms from getWaveformsSince(time).
	virtual QVariantList getTimesUntil(double time);
	virtual QVariantList getChannelsSince(double time);
	virtual QVariantList getChannelsUntil(double time);
	virtual QVariantList getUnitsSince(double time);
	virtual QVariantList getUnitsUntil(double time);
	virtual QVariantList getWaveformsSince(double time);
	virtual QVariantList getWaveformsUntil(double time);

private:

	struct SpikeData
	{
		SpikeData(){time_ = 0;channel_ = 0;unit_=0;waveform_.clear();};
		SpikeData(double time,int channel,int unit,QVariantList waveform){time_=time;channel_=channel;unit_=unit;waveform_=waveform;};
		inline bool operator<(const SpikeData& someData) const {
			return time_ < someData.time_;
		}
		double time_;
		int channel_;
		int unit_;
		QVariantList waveform_;
	};
	QVector<SpikeData> spikeData_;
	int maxChans_;
	int maxUnits_;
	int waveformSize_;
	QVariantList chans_;
	QVariantList units_;
};


}; //namespace Picto
#endif