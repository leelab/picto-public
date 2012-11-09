#ifndef _PLAYBACKINTERFACES_H_
#define _PLAYBACKINTERFACES_H_

#include <QVariantList>
#include <QStringList>
#include <QObject>

namespace Picto {
/*! \brief Provides interfaces usable by the outside world to gather data on the session state.
 *	We return all lists as QVaraintLists or QStringLists here so that they will be converted to 
 *  the javascript properly.
 */
class FrameReader : public QObject
{
	Q_OBJECT
public:
	virtual ~FrameReader(){};
	virtual double getPrevTime(){return 0;};
	virtual double getLatestTime(){return 0;};
	virtual double getNextTime(){return 0;};
	//Returns a list of frame times that occured with times > the input time and <= the current time.  
	virtual QVariantList getTimesSince(double time){return QVariantList();};
	//Returns a list of values with times > the current time and <= the input time.  
	virtual QVariantList getTimesUntil(double time){return QVariantList();};
};

class LfpReader : public QObject
{
	Q_OBJECT
public:
	virtual ~LfpReader(){};
	virtual QVariantList getChannels(){return QVariantList();};
	virtual double getSamplePeriod(){return 0;};
	virtual double getValue(int channel){return 0;};
	//Returns lfp values for the input channel with times > the input time.
	virtual QVariantList getValuesSince(int channel,double time){return QVariantList();};
	virtual QVariantList getValuesUntil(int channel,double time){return QVariantList();};
	//Returns lfp values for all channels (ordered like the result of getChannels())
	//with times > the current time and <= the input time.  Times should be incremented
	//by one getSamplePeriod() every getChannels().length() entries.
	virtual QVariantList getValuesSince(double time){return QVariantList();};
	virtual QVariantList getValuesUntil(double time){return QVariantList();};
};

class PropertyReader : public QObject
{
	Q_OBJECT
public:
	virtual ~PropertyReader(){};
	virtual QVariant getValue(){return QVariant();};
	//Returns a list of times when the property changed that occured with times > the 
	//input time and <= the current time.  
	virtual QVariantList getTimesSince(double time){return QVariantList();};
	//Returns a list of times when the property changed that occured with times > the 
	//current time and <= the input time.  
	virtual QVariantList getTimesUntil(double time){return QVariantList();};
	//Returns a property values to which the property was changed that occured with 
	//times > the input time and <= the current time.  There is a one to one matchup
	//of values from this function to times from the getTimesSince function.
	virtual QVariantList getValuesSince(double time){return QVariantList();};
	//Returns a property values to which the property was changed that occured with 
	//times > the current time and <= the input time.  There is a one to one matchup
	//of values from this function to times from the getTimesSince function.
	virtual QVariantList getValuesUntil(double time){return QVariantList();};
};

class RewardReader : public QObject
{
	Q_OBJECT
public:
	virtual ~RewardReader(){};
	virtual double getLatestTime(int channel){return 0;};
	virtual int getLatestDuration(int channel){return 0;};
	//Returns a list of reward times that occured with times > the input time and <= the current time.
	//From the input reward channel
	virtual QVariantList getTimesSince(int channel, double time){return QVariantList();};
	//Returns a list of reward times that occured with times > the current time and <= the input time.  
	//From the input reward channel
	virtual QVariantList getTimesUntil(int channel, double time){return QVariantList();};
	//Returns a list of reward durations that occured with times > the input time and <= the current time.
	//From the input reward channel.  There is a one to one matchup of values from this 
	//function to times from the getTimesSince function.
	virtual QVariantList getDurationsSince(int channel, double time){return QVariantList();};
	//Returns a list of reward durations that occured with times > the current time and <= the input time.  
	//From the input reward channel.  There is a one to one matchup of values from this 
	//function to times from the getTimesSince function.
	virtual QVariantList getDurationsUntil(int channel, double time){return QVariantList();};

};

class SignalReader : public QObject
{
	Q_OBJECT
public:
	virtual ~SignalReader(){};
	//Returns the sub channels of this signal
	virtual QStringList getChannels(){return QStringList();};	//Returns a list of times when transitions occured from the input source path regardless
	//of result with times > the input time and <= the current time.
	virtual QVariantList getTimesFromSourceSince(QString destPath, double time){return QVariantList();};
	//Returns a list of times when transitions occured from the input source path 
	//regardless of result with times > the current time and <= the input time.
	virtual QVariantList getTimesFromSourceUntil(QString destPath, double time){return QVariantList();};
	virtual double getSamplePeriod(){return 0;};
	virtual double getValue(QString channel){return 0;};
	//Returns signal values for the input sub channel with times > the input time.
	virtual QVariantList getValuesSince(QString channel,double time){return QVariantList();};
	virtual QVariantList getValuesUntil(QString channel,double time){return QVariantList();};
	//Returns signal values for all sub channels (ordered like the result of getChannels())
	//with times > the current time and <= the input time.  Times should be incremented
	//by one getSamplePeriod() every getChannels().length() entries.
	virtual QVariantList getValuesSince(double time){return QVariantList();};
	virtual QVariantList getValuesUntil(double time){return QVariantList();};
};

class SpikeReader : public QObject
{
	Q_OBJECT
public:
	virtual ~SpikeReader(){};
	virtual QVariantList getChannels(){return QVariantList();};
	virtual QVariantList getUnits(int channel){return QVariantList();};
	virtual double getLatestTime(int channel, int unit){return 0;};
	//Returns a list of spikes times when that occured with times > the input time and 
	//<= the current time on the input channel and unit.
	virtual QVariantList getTimesSince(int channel, int unit, double time){return QVariantList();};
	//Returns a list of spikes times when that occured with times > the current time and 
	// <= the input time on the input channel and unit.
	virtual QVariantList getTimesUntil(int channel, int unit, double time){return QVariantList();};
	//Returns a list of waveforms when that occured with times > the input time and 
	//<= the current time on the input channel and unit.  There is a one to one matchup of
	//values from this function and times from getTimesSince(channel,unit,time)
	virtual QVariantList getWaveformsSince(int channel, int unit, double time){return QVariantList();};
	//Returns a list of waveforms when that occured with times > the current time and 
	// <= the input time on the input channel and unit.  There is a one to one matchup of
	//values from this function and times from getTimesSince(channel,unit,time)
	virtual QVariantList getWaveformsUntil(int channel, int unit, double time){return QVariantList();};
	
	//Returns a list of spikes times when that occured with times > the input time and 
	//<= the current time on all channels and units.  There is a one to one matchup of times
	//from this function and channels from getChannelsSince(time), units from getUnitsSince(time)
	//and waveforms from getWaveformsSince(time).
	virtual QVariantList getTimesSince(double time){return QVariantList();};
	//Returns a list of spikes times when that occured with times > the current time and 
	// <= the input time on the all channels and units.  There is a one to one matchup of times
	//from this function and channels from getChannelsUntil(time), units from getUnitsUntil(time)
	//and waveforms from getWaveformsSince(time).
	virtual QVariantList getTimesUntil(double time){return QVariantList();};
	virtual QVariantList getChannelsSince(double time){return QVariantList();};
	virtual QVariantList getChannelsUntil(double time){return QVariantList();};
	virtual QVariantList getUnitsSince(double time){return QVariantList();};
	virtual QVariantList getUnitsUntil(double time){return QVariantList();};
	virtual QVariantList getWaveformsSince(double time){return QVariantList();};
	virtual QVariantList getWaveformsUntil(double time){return QVariantList();};
};

class TransitionReader : public QObject
{
	Q_OBJECT
public:
	virtual ~TransitionReader(){};
	virtual QString getLatestSourceName(){return "";};
	virtual QString getLatestSourcePath(){return "";};
	virtual QString getLatestResultName(){return "";};
	virtual QString getLatestResultPath(){return "";};
	virtual QString getLatestDestination(){return "";};
	virtual QString getLatestDestinationPath(){return "";};
	//Returns a list of times when transitions occured with times > the 
	//input time and <= the current time.  There is a one to one matchup of times
	//from this function and values from getSourcePathsSince(time),getResultPathsSince(time)
	//getDestinationsSince(time).
	virtual QVariantList getTimesSince(double time){return QVariantList();};
	//Returns a list of times when transitions occured with times > the 
	//current time and <= the input time.  There is a one to one matchup of times
	//from this function and values from getSourcePathsUntil(time),getResultPathsUntil(time)
	//getDestinationPathsUntiltime).
	virtual QVariantList getTimesUntil(double time){return QVariantList();};
	virtual QVariantList getSourcePathsSince(double time){return QVariantList();};
	virtual QVariantList getSourcePathsUntil(double time){return QVariantList();};
	virtual QVariantList getResultPathsSince(double time){return QVariantList();};
	virtual QVariantList getResultPathsUntil(double time){return QVariantList();};
	virtual QVariantList getDestinationPathsSince(double time){return QVariantList();};
	virtual QVariantList getDestinationPathsUntil(double time){return QVariantList();};

	//Returns a list of times when a transition occured from the input source path and 
	//result with times > the input time and <= the current time.
	virtual QVariantList getTimesSince(QString sourcePath, QString resultName, double time){return QVariantList();};
	//Returns a list of times when transitions occured from the input source path and result
	//with times > the current time and <= the input time.
	virtual QVariantList getTimesUntil(QString sourcePath, QString resultName, double time){return QVariantList();};
	//Returns a list of times when transitions occured from the input source path regardless
	//of result with times > the input time and <= the current time.
	virtual QVariantList getTimesFromSourceSince(QString destPath, double time){return QVariantList();};
	//Returns a list of times when transitions occured from the input source path 
	//regardless of result with times > the current time and <= the input time.
	virtual QVariantList getTimesFromSourceUntil(QString destPath, double time){return QVariantList();};
	//Returns a list of times when transitions occured to the input destination path with times > the 
	//input time and <= the current time.
	virtual QVariantList getTimesToDestSince(QString destPath, double time){return QVariantList();};
	//Returns a list of times when transitions occured to the input destination with times > the 
	//current time and <= the input time.
	virtual QVariantList getTimesToDestUntil(QString destPath, double time){return QVariantList();};
};

}; //namespace Picto
#endif