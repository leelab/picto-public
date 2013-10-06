#ifndef _SPIKESTATE_H_
#define _SPIKESTATE_H_
#include <QVector>
#include <QHash>
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackSpikeData;
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class SpikeState :public SpikeReader, public DataState
{
	Q_OBJECT
public:
	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual PlaybackIndex getCurrentIndex();
	virtual PlaybackIndex getNextIndex(double lookForwardTime);
	virtual void moveToIndex(PlaybackIndex index);

	//Spike Reader Interface
	virtual QVariantList getChannels();
	virtual QVariantList getUnits(int channel);
	virtual double getSamplePeriod();
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

signals:
	void spikeEvent(double time, int channel, int unit, QVector<float> waveform);
private:
	PlaybackIndex getNextIndex();
	PlaybackIndex globalTimeToRunIndex(double time);
	QSqlDatabase session_;
	QSharedPointer<QSqlQuery> query_;
	double runStart_;
	double runEnd_;
	int curr_;
	double sampPeriod_;
	QVariantList channels_;
	QHash<int,QVariantList> unitsLookup_;
	QVector<PlaybackSpikeData> data_;
};

struct PlaybackSpikeData
{
	PlaybackSpikeData(){};
	PlaybackSpikeData(double time,int channel, int unit, QVector<float> waveform){time_=time;channel_=channel;unit_=unit;waveform_ = waveform;};
	inline bool operator<(const PlaybackSpikeData& someData) const {
		return time_ < someData.time_;
	}
	double time_;
	int channel_;
	int unit_;
	QVector<float> waveform_;
};

}; //namespace Picto
#endif