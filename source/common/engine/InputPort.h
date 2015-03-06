#ifndef INPUTPORT_H
#define INPUTPORT_H

#include <QHash>
#include <QWeakPointer>
#include <QSharedPointer>
#include <QVector>
#include "../../common/common.h"
namespace Picto {

/*! \brief Used to collect data from a single port that may handle multiple signals.
 *	
 *	Since in many cases different input lines on a single DAQ device port cannot be configured to be sampled at different sample rates,
 *	this class handles finding a sample rate that is high enough that it can be used to provide the desired sample rate for all desired 
 *	input channels, and then down sampling the resultant data to provide data at those desired sample rates.  Another common issue with 
 *	DAQ devices is that their clocks are fairly innaccurate (apparently this is the trade off for being able to set whatever sample rate
 *	you want).  These inaccurate sample clocks can create significant timing errors over the even a few seconds.  This class handles 
 *	synchronization of the DAQ sample clock and the Picto clock.  It does this by comparing the calculated time of the incoming data 
 *	assuming a perfect DAQ clock and comparing it to the time which the data was read according to the Picto clock.  If the calculated
 *	clock dictates that the last data sample was read significantly before the time read on the Picto clock, data points are duplicated.
 *	If on the other hand, the calculated clock dictates that the last data point was read significantly after the time reported by the
 *	Picto clock, data points at the end are deleted.  To keep from deleting significant information, we always sample the DAQ at 4 times the
 *	maximum necessary rate, so that we never need to throw out more than a quarter of a single sample period.  This means that data reported
 *	by this class will always have been read within a quarter sample period of when it says it was such that we will be working within the
 *	set margin of error.
 *
 *	Child classes should handle the actual setup of the underlying device and implement the class's virtual functions.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API InputPort
#else
class InputPort
#endif
{
public:
	InputPort();
	virtual ~InputPort();
	
	void addInputChannel(int channelNum, int msPerSample);

	void enable(bool en);

	void updateDataBuffer(double currTimestamp);
	bool hasNewData(int channelNum);
	QVector<double> getData(int channelNum);
	double getFrameToSampleOffset(int channelNum);
	/*! \brief Returns a string with the name of this type of InputPort.*/
	virtual QString type(){return "default";};

protected:
	double deviceSampRate_;							//!< This is the rate at which the Daq device will be sampled.
	QVector<int> channelNums_;						//!< A vector containing the channels to be sampled by the Daq device
	QHash<int, QVector<double>> channelBuffers_;	//!< A hash of data buffers indexed by channel number to be filled by the child class
	virtual bool startSampling() = 0;				//!< Starts sampling of the DAQ hardware.  Returns false if sampling can't be started.
	virtual void stopSampling() = 0;				//!< Stops sampling of the DAQ hardware.  Returns false if sampling can't be stopped.
	virtual double updateDataBuffer() = 0;			//!< Loads channelBuffers_ with data, returns the precise time when the data was actually read.
private:

	bool start();
	//maps channel num to samplerate
	QHash<int, int> sampResMap_;
	QHash<int,double> latestFrameOffsetTimes_;
	QHash<int, int> sampsPerReadMap_;
	QHash<int,int> channelPhaseMap_;
	QHash<int, bool> polledDataMap_;
	QHash<int, QVector<double>> channelLeftOvers;	//!< A hash of data buffers indexed by channel number with data that will be used on the next updateDataBuffer(double) call.
	bool taskRunning_;
	int sampsCollected_;
	bool needsUpdate_;
	double lastSyncTimestamp_;
	bool enabled_;

};

};

#endif