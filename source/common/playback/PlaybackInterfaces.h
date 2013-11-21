#ifndef _PLAYBACKINTERFACES_H_
#define _PLAYBACKINTERFACES_H_

#include <QVariantList>
#include <QStringList>
#include <QObject>
#include "../common.h"

namespace Picto {
/*! \brief Provides interfaces usable by the outside world to gather data on the session state.
 *	We return all lists as QVaraintLists or QStringLists here so that they will be converted to 
 *  the javascript properly.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API FrameReader : public QObject
#else
class FrameReader : public QObject
#endif
{
	Q_OBJECT
public:
	virtual ~FrameReader(){};
	virtual double getPrevTime()=0;
	virtual double getLatestTime()=0;
	virtual double getNextTime()=0;
	//Returns a list of frame times that occured with times > the input time and <= the current time.  
	virtual QVariantList getTimesSince(double time)=0;
	//Returns a list of values with times > the current time and <= the input time.  
	virtual QVariantList getTimesUntil(double time)=0;
};

#if defined WIN32 || defined WINCE
class PICTOLIB_API LfpReader : public QObject
#else
class LfpReader : public QObject
#endif
{
	Q_OBJECT
public:
	virtual ~LfpReader(){};
	virtual QVariantList getChannels()=0;
	virtual double getSamplePeriod()=0;
	virtual double getLatestTime()=0;
	virtual double getLatestValue(int channel)=0;
	virtual double getNextTime()=0;
	virtual double getNextValue(int channel)=0;
	//Returns lfp values for the input channel with times > the input time.
	virtual QVariantList getValuesSince(int channel,double time)=0;
	virtual QVariantList getValuesUntil(int channel,double time)=0;
	virtual QVariantList getTimesSince(double time)=0;
	virtual QVariantList getTimesUntil(double time)=0;
};

class PropertyReader : public QObject
{
//	Q_OBJECT
//public:
//	virtual ~PropertyReader(){};
//	virtual QVariant getValue(){return QVariant();};
//	//Returns a list of times when the property changed that occured with times > the 
//	//input time and <= the current time.  
//	virtual QVariantList getTimesSince(double time){return QVariantList();};
//	//Returns a list of times when the property changed that occured with times > the 
//	//current time and <= the input time.  
//	virtual QVariantList getTimesUntil(double time){return QVariantList();};
//	//Returns a property values to which the property was changed that occured with 
//	//times > the input time and <= the current time.  There is a one to one matchup
//	//of values from this function to times from the getTimesSince function.
//	virtual QVariantList getValuesSince(double time){return QVariantList();};
//	//Returns a property values to which the property was changed that occured with 
//	//times > the current time and <= the input time.  There is a one to one matchup
//	//of values from this function to times from the getTimesSince function.
//	virtual QVariantList getValuesUntil(double time){return QVariantList();};
};

#if defined WIN32 || defined WINCE
class PICTOLIB_API RewardReader : public QObject
#else
class RewardReader : public QObject
#endif
{
	Q_OBJECT
public:
	virtual ~RewardReader(){};
	virtual double getLatestTime()=0;
	virtual double getNextTime()=0;
	virtual int getLatestDuration()=0;
	virtual int getNextDuration()=0;
	//Returns a list of reward times that occured with times > the input time and <= the current time.
	//From the input reward channel
	virtual QVariantList getTimesSince(double time)=0;
	//Returns a list of reward times that occured with times > the current time and <= the input time.  
	//From the input reward channel
	virtual QVariantList getTimesUntil(double time)=0;
	//Returns a list of reward durations that occured with times > the input time and <= the current time.
	//From the input reward channel.  There is a one to one matchup of values from this 
	//function to times from the getTimesSince function.
	virtual QVariantList getDurationsSince(double time)=0;
	//Returns a list of reward durations that occured with times > the current time and <= the input time.  
	//From the input reward channel.  There is a one to one matchup of values from this 
	//function to times from the getTimesSince function.
	virtual QVariantList getDurationsUntil(double time)=0;

};

#if defined WIN32 || defined WINCE
class PICTOLIB_API RunNotesReader : public QObject
#else
class RunNotesReader : public QObject
#endif
{
	Q_OBJECT
public:
	virtual ~RunNotesReader(){};
	virtual QString getName()=0;
	virtual double getStartTime()=0;
	virtual double getEndTime()=0;
	virtual QString getNotes()=0;
	virtual int getRunIndex()=0;
};

#if defined WIN32 || defined WINCE
class PICTOLIB_API SignalReader : public QObject
#else
class SignalReader : public QObject
#endif
{
	Q_OBJECT
public:
	virtual ~SignalReader(){};
	//Returns the name of the signal handled by this signal reader
	virtual QString getName()=0;
	//Returns the sub channels of this signal
	virtual QStringList getComponentNames()=0;
	virtual double getSamplePeriod()=0;
	virtual double getLatestTime()=0;
	virtual double getLatestValue(QString channel)=0;
	virtual double getNextTime()=0;
	virtual double getNextValue(QString channel)=0;
	//Returns a list of signal read times that occured > the input time and 
	//<= the current time.  There is a one to one matchup of times
	//from this function and values from getValuesSince(channel,time).
	virtual QVariantList getTimesSince(double time)=0;
	//Returns a list of signal read times that occured > the current time and 
	// <= the input time.  There is a one to one matchup of times
	//from this function and values from getValuesUntil(channel,time).
	virtual QVariantList getTimesUntil(double time)=0;
	//Returns signal values for the input sub channel with times > the input time and
	//<= the current time.  There is a one to one matchup of times
	//from this function and values from getTimesSince(channel,time).
	virtual QVariantList getValuesSince(QString channel,double time)=0;
	//Returns signal values for the input sub channel with times > the current time and
	//<= the input time time.  There is a one to one matchup of times
	//from this function and values from getTimesUntil(channel,time).
	virtual QVariantList getValuesUntil(QString channel,double time)=0;
};

#if defined WIN32 || defined WINCE
class PICTOLIB_API SpikeReader : public QObject
#else
class SpikeReader : public QObject
#endif
{
	Q_OBJECT
public:
	virtual ~SpikeReader(){};
	virtual QVariantList getChannels()=0;
	virtual QVariantList getUnits(int channel)=0;
	virtual double getSamplePeriod()=0;
	virtual double getLatestTime()=0;
	virtual int getLatestChannel()=0;
	virtual int getLatestUnit()=0;
	virtual QVariantList getLatestWaveform()=0;
	virtual double getNextTime()=0;
	virtual int getNextChannel()=0;
	virtual int getNextUnit()=0;
	virtual QVariantList getNextWaveform()=0;
	
	//Returns a list of spikes times when that occured with times > the input time and 
	//<= the current time on all channels and units.  There is a one to one matchup of times
	//from this function and channels from getChannelsSince(time), units from getUnitsSince(time)
	//and waveforms from getWaveformsSince(time).
	virtual QVariantList getTimesSince(double time)=0;
	//Returns a list of spikes times when that occured with times > the current time and 
	// <= the input time on the all channels and units.  There is a one to one matchup of times
	//from this function and channels from getChannelsUntil(time), units from getUnitsUntil(time)
	//and waveforms from getWaveformsSince(time).
	virtual QVariantList getTimesUntil(double time)=0;
	virtual QVariantList getChannelsSince(double time)=0;
	virtual QVariantList getChannelsUntil(double time)=0;
	virtual QVariantList getUnitsSince(double time)=0;
	virtual QVariantList getUnitsUntil(double time)=0;
	virtual QVariantList getWaveformsSince(double time)=0;
	virtual QVariantList getWaveformsUntil(double time)=0;
};

class TransitionReader : public QObject
{
	Q_OBJECT
public:
	//virtual ~TransitionReader(){};
	//virtual QString getLatestSourceName(){return "";};
	//virtual QString getLatestSourcePath(){return "";};
	//virtual QString getLatestResultName(){return "";};
	//virtual QString getLatestResultPath(){return "";};
	//virtual QString getLatestDestination(){return "";};
	//virtual QString getLatestDestinationPath(){return "";};
	////Returns a list of times when transitions occured with times > the 
	////input time and <= the current time.  There is a one to one matchup of times
	////from this function and values from getSourcePathsSince(time),getResultPathsSince(time)
	////getDestinationsSince(time).
	//virtual QVariantList getTimesSince(double time){return QVariantList();};
	////Returns a list of times when transitions occured with times > the 
	////current time and <= the input time.  There is a one to one matchup of times
	////from this function and values from getSourcePathsUntil(time),getResultPathsUntil(time)
	////getDestinationPathsUntiltime).
	//virtual QVariantList getTimesUntil(double time){return QVariantList();};
	//virtual QVariantList getSourcePathsSince(double time){return QVariantList();};
	//virtual QVariantList getSourcePathsUntil(double time){return QVariantList();};
	//virtual QVariantList getResultPathsSince(double time){return QVariantList();};
	//virtual QVariantList getResultPathsUntil(double time){return QVariantList();};
	//virtual QVariantList getDestinationPathsSince(double time){return QVariantList();};
	//virtual QVariantList getDestinationPathsUntil(double time){return QVariantList();};

	////Returns a list of times when a transition occured from the input source path and 
	////result with times > the input time and <= the current time.
	//virtual QVariantList getTimesSince(QString sourcePath, QString resultName, double time){return QVariantList();};
	////Returns a list of times when transitions occured from the input source path and result
	////with times > the current time and <= the input time.
	//virtual QVariantList getTimesUntil(QString sourcePath, QString resultName, double time){return QVariantList();};
	////Returns a list of times when transitions occured from the input source path regardless
	////of result with times > the input time and <= the current time.
	//virtual QVariantList getTimesFromSourceSince(QString destPath, double time){return QVariantList();};
	////Returns a list of times when transitions occured from the input source path 
	////regardless of result with times > the current time and <= the input time.
	//virtual QVariantList getTimesFromSourceUntil(QString destPath, double time){return QVariantList();};
	////Returns a list of times when transitions occured to the input destination path with times > the 
	////input time and <= the current time.
	//virtual QVariantList getTimesToDestSince(QString destPath, double time){return QVariantList();};
	////Returns a list of times when transitions occured to the input destination with times > the 
	////current time and <= the input time.
	//virtual QVariantList getTimesToDestUntil(QString destPath, double time){return QVariantList();};
};

}; //namespace Picto
#endif