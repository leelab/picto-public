#ifndef INPUTPORT_H
#define INPUTPORT_H

#include <QHash>
#include <QWeakPointer>
#include <QSharedPointer>
#include <QVector>
#include "../../common/common.h"
namespace Picto {

/*! \brief Used by to collect data from a single port that may handle multiple signals.
 *	
 *	Since, in many cases each port on a DAQ device cannot be configured to sample seperate lines differently
 *	this port handles merging of multiple data channels onto one port.  This class also handles synchronization
 *	of the incoming data samples to the Picto clock by duplicating or throwing out individual samples at a
 *	higher sample rate than required by individual channels when clocks fall out of sync.
 *	Child classes handle the actual setup of the underlying device.
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
	virtual QString type(){return "default";};

protected:
	double deviceSampRate_;		//! This is the rate at which the Daq device will be sampled.
	QVector<int> channelNums_;	//! A vector containing the channels to be sampled by the Daq device
	QHash<int, QVector<double>> channelBuffers_;	//! A hash of data buffers indexed by channel number to be filled by the child class
	virtual bool startSampling() = 0;
	virtual void stopSampling() = 0;
	virtual double updateDataBuffer() = 0;	//! Loads channelBuffers_ with data, returning the time the data was actually read.
private:

	bool start();
	//maps channel num to samplerate
	QHash<int, int> sampResMap_;
	QHash<int,double> latestFrameOffsetTimes_;
	QHash<int, int> sampsPerReadMap_;
	QHash<int,int> channelPhaseMap_;
	QHash<int, bool> polledDataMap_;
	QHash<int, QVector<double>> channelLeftOvers;	//! A hash of data buffers indexed by channel number with data that will be used on the next updateDataBuffer(double) call.
	bool taskRunning_;
	int sampsCollected_;
	bool needsUpdate_;
	double lastSyncTimestamp_;
	bool enabled_;

};

};

#endif