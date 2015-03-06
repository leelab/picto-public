#ifndef _PLAYBACKINTERFACES_H_
#define _PLAYBACKINTERFACES_H_

#include <QVariantList>
#include <QStringList>
#include <QObject>
#include "../common.h"

namespace Picto {
/*! \file PlaybackInterfaces.h
 *	Provides interfaces usable by the outside world to gather data on the session state.
 *	We return all lists as QVaraintLists or QStringLists here so that they will be converted to 
 *  the javascript properly.
 */

/*! \brief An base class for classes that can provide Frame data over input time ranges.
 *	\details This class is used with the Analysis system so that Analyses can gather data over
 *	a range of times surrounding a current control state.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	/*!	\brief Returns the time of the frame shown before the latest one.*/
	virtual double getPrevTime()=0;
	/*!	\brief Returns the time of the latest frame to have been shown.*/
	virtual double getLatestTime()=0;
	/*!	\brief Returns the time of the next frame to be shown.*/
	virtual double getNextTime()=0;
	/*! \brief Returns a list of frame times that occured with times > the input time and <= the current time.  */
	virtual QVariantList getTimesSince(double time)=0;
	/*! \brief Returns a list of values with times > the current time and <= the input time.  */
	virtual QVariantList getTimesUntil(double time)=0;
};

/*! \brief An base class for classes that can provide Local Field Potential data over input time ranges.
 *	\details This class is used with the Analysis system so that Analyses can gather data over
 *	a range of times surrounding a current control state.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API LfpReader : public QObject
#else
class LfpReader : public QObject
#endif
{
	Q_OBJECT
public:
	virtual ~LfpReader(){};
	/*! \brief Returns a list of the channels on which lfp data was captured during this run.*/
	virtual QVariantList getChannels()=0;
	/*! \brief Returns the sample period for lfp data capture.*/
	virtual double getSamplePeriod()=0;
	/*! \brief Returns the time of the latest lfp data sample.*/
	virtual double getLatestTime()=0;
	/*! \brief Returns the latest lfp value on the channel.*/
	virtual double getLatestValue(int channel)=0;
	/*! \brief Returns the time at which the next signal will be read.*/
	virtual double getNextTime()=0;
	/*! \brief Returns the next value that will be read for the input channel.*/
	virtual double getNextValue(int channel)=0;
	/*! \brief Returns a list of lfp values for the input channel that occured with times > the input time and <= the current time.*/
	virtual QVariantList getValuesSince(int channel,double time)=0;
	/*! \brief Returns a list of lfp values for the input channel that occured with times > the current time and <= the input time.*/
	virtual QVariantList getValuesUntil(int channel,double time)=0;
	/*! \brief Returns a list of lfp sample times that occured with times > the input time and <= the current time.*/
	virtual QVariantList getTimesSince(double time)=0;
	/*! \brief Returns a list of lfp sample times that occured with times > the current time and <= the input time.  */
	virtual QVariantList getTimesUntil(double time)=0;
};

/*! \brief An base class for classes that can provide Reward data over input time ranges.
 *	\details This class is used with the Analysis system so that Analyses can gather data over
 *	a range of times surrounding a current control state.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API RewardReader : public QObject
#else
class RewardReader : public QObject
#endif
{
	Q_OBJECT
public:
	virtual ~RewardReader(){};
	/*! \brief Returns the time of the latest reward to have been supplied.*/
	virtual double getLatestTime()=0;
	/*! \brief Returns the time of the next reward to be supplied.*/
	virtual double getNextTime()=0;
	/*! \brief Returns the duration of the latest reward to have been supplied.*/
	virtual int getLatestDuration()=0;
	/*! \brief Returns the duration of the next reward to be supplied.*/
	virtual int getNextDuration()=0;
	/*! \brief Returns a list of reward times that occured with times > the input time and <= the current time.
	 *	From the input reward channel*/
	virtual QVariantList getTimesSince(double time)=0;
	/*! \brief Returns a list of reward times that occured with times > the current time and <= the input time.  
	 *	From the input reward channel.*/
	virtual QVariantList getTimesUntil(double time)=0;
	/*! \brief Returns a list of reward durations that occured with times > the input time and <= the current time.
	 *	From the input reward channel.  There is a one to one matchup of values from this 
	 *	function to times from the getTimesSince() function.*/
	virtual QVariantList getDurationsSince(double time)=0;
	/*! \brief Returns a list of reward durations that occured with times > the current time and <= the input time.  
	 *	From the input reward channel.  There is a one to one matchup of values from this 
	 *	function to times from the getTimesUntil() function.*/
	virtual QVariantList getDurationsUntil(double time)=0;

};

/*! \brief An base class for classes that can provide Run Notes data.
 *	\details This class allows Analyses to gather data about the current run, so that
 *	it can be used to provide meta-data in Analysis outputs.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API RunNotesReader : public QObject
#else
class RunNotesReader : public QObject
#endif
{
	Q_OBJECT
public:
	virtual ~RunNotesReader(){};
	/*!	\brief Gets the name of the current run.
	 *	\details This name is either a default one based on the start time of the run, 
	 *	or the one set by the operator in the run info section during a session.
	 */
	virtual QString getName()=0;
	/*! \brief Returns the time that this run started in seconds where time zero is the 
	 *	beginning of the session.
	 */
	virtual double getStartTime()=0;
	/*! \brief Returns the time that this run ends in seconds where time zero is the 
	 *	beginning of the session.
	 */
	virtual double getEndTime()=0;
	/*! \brief Returns the notes string that was saved by the operator during 
	 *	the session.  If no notes were saved, an empty string is returned.
	 */
	virtual QString getNotes()=0;
	/*! \brief Returns the index of this run in this sessions list of task runs, 
	 *	where the first run has an index of zero.
	 */
	virtual int getRunIndex()=0;
};

/*! \brief An base class for classes that can provide Input Signal data over input time ranges (ie. Position, Eye Diameter).
 *	\details This class is used with the Analysis system so that Analyses can gather data over
 *	a range of times surrounding a current control state.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
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
	/*! \brief Returns a list of the signal's sub component names, ie. x,y*/
	virtual QStringList getComponentNames()=0;
	/*! \brief Returns the sample period for this signal in seconds.*/
	virtual double getSamplePeriod()=0;
	/*! \brief Returns the time of the latest signal to have been read in seconds*/
	virtual double getLatestTime()=0;
	/*! \brief Returns the latest value of the input signal subchannel*/
	virtual double getLatestValue(QString channel)=0;
	/*! \brief Returns the time that the signal will be next read.*/
	virtual double getNextTime()=0;
	/*! \brief Returns the next value that will be read for the input signal subchannel.*/
	virtual double getNextValue(QString channel)=0;
	/*! \brief Returns a list of signal read times that occured > the input time and 
	 *	<= the current time.  There is a one to one matchup of times
	 *	from this function and values from getValuesSince(channel,time).*/
	virtual QVariantList getTimesSince(double time)=0;
	/*! \brief Returns a list of signal read times that occured > the current time and 
	 *	<= the input time.  There is a one to one matchup of times
	 *	from this function and values from getValuesUntil(channel,time).*/
	virtual QVariantList getTimesUntil(double time)=0;
	/*! \brief Returns signal values for the input sub channel with times > the input time and
	 *	<= the current time.  There is a one to one matchup of times
	 *	from this function and values from getTimesSince(channel,time).*/
	virtual QVariantList getValuesSince(QString channel,double time)=0;
	/*! \brief Returns signal values for the input sub channel with times > the current time and
	 *	<= the input time time.  There is a one to one matchup of times
	 *	from this function and values from getTimesUntil(channel,time).*/
	virtual QVariantList getValuesUntil(QString channel,double time)=0;
};

/*! \brief An base class for classes that can provide Spike data over input time ranges.
 *	\details This class is used with the Analysis system so that Analyses can gather data over
 *	a range of times surrounding a current control state.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SpikeReader : public QObject
#else
class SpikeReader : public QObject
#endif
{
	Q_OBJECT
public:
	virtual ~SpikeReader(){};
	/*! \brief Returns a list of the channels on which spikes occured during this run.*/
	virtual QVariantList getChannels()=0;
	/*! \brief Returns a list of the units on which spikes occured for the input channel during this run.*/
	virtual QVariantList getUnits(int channel)=0;
	/*! \brief Returns the sample period in seconds used by the spike data acquisition system*/
	virtual double getSamplePeriod()=0;
	/*! \brief Returns the time of the latest spike to have been read.*/
	virtual double getLatestTime()=0;
	/*! \brief Returns the channel of the latest spike to have been read.*/
	virtual int getLatestChannel()=0;
	/*! \brief Returns the unit of the latest spike to have been read.*/
	virtual int getLatestUnit()=0;
	/*! \brief Returns the waveform of the latest spike to have been read.*/
	virtual QVariantList getLatestWaveform()=0;
	/*! \brief Returns the time of the next spike to be read.*/
	virtual double getNextTime()=0;
	/*! \brief Returns the channel of the next spike to be read.*/
	virtual int getNextChannel()=0;
	/*! \brief Returns the unit of the next spike to be read.*/
	virtual int getNextUnit()=0;
	/*! \brief Returns the waveform of the next spike to be read.*/
	virtual QVariantList getNextWaveform()=0;
	
	/*! \brief Returns a list of spike times that occured with times > the input time and 
	 *	<= the current time on all channels and units.  There is a one to one matchup of times
	 *	from this function and channels from getChannelsSince(), units from getUnitsSince()
	 *	and waveforms from getWaveformsSince().*/
	virtual QVariantList getTimesSince(double time)=0;
	/*! \brief Returns a list of spike times when that occured with times > the current time and 
	 *	<= the input time on the all channels and units.  There is a one to one matchup of times
	 *	from this function and channels from getChannelsUntil(), units from getUnitsUntil()
	 *	and waveforms from getWaveformsSince().*/
	virtual QVariantList getTimesUntil(double time)=0;
	/*! \brief Returns a list of channels for spikes that occured with times > the input time and 
	 *	<= the current time on all channels and units.  There is a one to one matchup of channels
	 *	from this function and times from getTimesSince(), units from getUnitsSince()
	 *	and waveforms from getWaveformsSince().*/
	virtual QVariantList getChannelsSince(double time)=0;
	/*! \brief Returns a list of channels for spikes that occured with times > the current time and 
	 *	<= the input time on the all channels and units.  There is a one to one matchup of channels
	 *	from this function and times from getTimesUntil(), units from getUnitsUntil()
	 *	and waveforms from getWaveformsSince().*/
	virtual QVariantList getChannelsUntil(double time)=0;
	/*! \brief Returns a list of units for spikes that occured with times > the input time and 
	 *	<= the current time on all channels and units.  There is a one to one matchup of units
	 *	from this function and channels from getChannelsSince(), times from getTimesSince()
	 *	and waveforms from getWaveformsSince().*/
	virtual QVariantList getUnitsSince(double time)=0;
	/*! \brief Returns a list of units for spikes that occured with times > the current time and 
	 *	<= the input time on the all channels and units.  There is a one to one matchup of units
	 *	from this function and channels from getChannelsUntil(), times from getTimesUntil()
	 *	and waveforms from getWaveformsSince().*/
	virtual QVariantList getUnitsUntil(double time)=0;
	/*! \brief Returns a list of waveforms for spikes that occured with times > the input time and 
	 *	<= the current time on all channels and units.  There is a one to one matchup of waveforms
	 *	from this function and channels from getChannelsSince(), units from getUnitsSince()
	 *	and times from getTimesSince().*/
	virtual QVariantList getWaveformsSince(double time)=0;
	/*! \brief Returns a list of waveforms for spikes that occured with times > the current time and 
	 *	<= the input time on the all channels and units.  There is a one to one matchup of waveforms
	 *	from this function and channels from getChannelsUntil(), units from getUnitsUntil()
	 *	and times from getTimesUntil().*/
	virtual QVariantList getWaveformsUntil(double time)=0;
};

}; //namespace Picto
#endif