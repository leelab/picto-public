#ifndef _SPIKESTATE_H_
#define _SPIKESTATE_H_
#include <QVector>
#include <QHash>
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackSpikeData;
/*! \brief Implements the DataState and SpikeReader classes to load a Picto Session database, extract neural spike data
 *	and implement functions for traversing through that data.
 *	\details The class is fairly simple, a QList of PlaybackSpikeData objects is loaded from the session data.  Each
 *	PlaybackSpikeData represents a single neural spike and when moveToIndex() is called, we just traverse through the
 *	list until we reach a PlaybackSpikeData with the appropriate time.  Each time moveToIndex() causes us to pass through
 *	a PlaybackSpikeData entry, the spikeEvent() signal is called, which tells the rest of the playback system that a
 *	neural spike occured.
 *
 *	\note Since the functions here simply implement the SpikeReader and DataState classes for data read in from a Session
 *	Database, there is not much to add in terms of documentation beyond what was described above, so we will not be adding
 *	function level documentation for this class.
 *	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2017
 */
class SpikeState : public SpikeReader, public DataState
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
	
	virtual QVariantList getTimesSince(double time);
	virtual QVariantList getTimesUntil(double time);
	virtual QVariantList getChannelsSince(double time);
	virtual QVariantList getChannelsUntil(double time);
	virtual QVariantList getUnitsSince(double time);
	virtual QVariantList getUnitsUntil(double time);
	virtual QVariantList getWaveformsSince(double time);
	virtual QVariantList getWaveformsUntil(double time);

	virtual QVariantList getSpikeDataSince(double time);
signals:
	/*! \brief Emitted whenever a PlaybackSpikeData object is traversed due to a call to moveToIndex().
	 *	\details time is the time at which the spike occured, channel is the electrode channel on which it occured, unit
	 *	is the neuron unit within that channel that spiked, and waveform is a vector of floats describing the spike
	 *	waveform, where the time between each float is getSamplePeriod().
	 */
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
	QList<PlaybackSpikeData> data_;	
};

/*! \brief A struct used to store data for a single Neural spike.
 *	\details Includes the time at which the spike occured, the channel on which it occured and the unit number of the
 *	spiking neuron within the channel.  Also includes a vector of floats describing the spike waveform.
 */
struct PlaybackSpikeData
{
	PlaybackSpikeData(){};
	PlaybackSpikeData(double time,int channel, int unit, QVector<float> waveform){time_=time;channel_=channel;unit_=unit;waveform_ = waveform;};
	PlaybackSpikeData(const PlaybackSpikeData& other){
		time_ = other.time_;
		channel_ = other.channel_;
		unit_ = other.unit_;
		waveform_ = other.waveform_;
	};
	~PlaybackSpikeData(){};
	//! One PlaybackSpikeData is lower than another if its time_ is lower.
	inline bool operator<(const PlaybackSpikeData& someData) const {
		return time_ < someData.time_;
	}
	double time_;
	int channel_;
	int unit_;
	QVector<float> waveform_;
};
Q_DECLARE_METATYPE(PlaybackSpikeData)
}; //namespace Picto
#endif