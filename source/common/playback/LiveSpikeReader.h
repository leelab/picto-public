#ifndef _LiveSpikeReader_H_
#define _LiveSpikeReader_H_
#include <QVector>
#include "../../common/playback/PlaybackInterfaces.h"

namespace Picto {
/*! \brief Implements the SpikeReader class for a live test experiment.
 *	\details Since this is meant to be used with a test experiment, this object just generates fake spike data.  Every
 *	time createVirtualSpike() is called, it creates a fake spike with the input time and a random channel/unit between 0
 *	and the maxChan/maxUnit input in the constructor.  Waveforms are also virtual with a simple noisy sine wave shape.
 *	Since this object is filled during run time, Analyses have access to all past data but no future data.  Requests for
 *	future data from this class will result in meaningless values.
 *	\note Since the functions here simply implement the SpikeReader class for live session data, there is not much to add
 *	in terms of documentation beyond what was described above, so we will not be adding additional function level
 *	documentation for many of the functions in this class.
 *	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2017
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
	void spikeEvent(double time, int channel, int unit, QVariantList waveform);

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