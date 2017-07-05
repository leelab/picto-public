#include "InputPort.h"

#include "../../common/timing/Timestamper.h"
#include "../../common/memleakdetect.h"

using namespace Picto;


/*! \brief This is a Recursive Greatest Common Divisor function for use in calculating the nidaqSampleRate.
 *	Pretty cool right?!  See http://www.phailed.me/2011/02/greatest-common-denominator/
 */
int gcd(int high,int low)
{
	if(!low)
		return high;
	return gcd(low,high%low);
}

InputPort::InputPort()
{
	needsUpdate_ = true;
	deviceSampRate_ = 0;
	taskRunning_ = false;
	lastSyncTimestamp_ = -1;
	sampsCollected_ = 0;
	enabled_ = false;
}

InputPort::~InputPort() 
{ 
}

/*! \brief Adds a new input channel to this InputPort that is to be sampled at the input number of msPerSample.
 *	\details Since the synchronization algorithm used by this InputPort requires that we sample at 4 times the fastest
 *	sample rate, we take the Greatest Common Divisor of all channels' sample periods, divide it by 4 and sample at that
 *	sample period.
 */
void InputPort::addInputChannel(int channelNum, int msPerSample)
{
	Q_ASSERT(!sampResMap_.contains(channelNum));
	sampResMap_[channelNum] = msPerSample;
	//Since this channel was just added, its as if it was just polled.
	polledDataMap_[channelNum] = true;
	channelPhaseMap_[channelNum] = 0;
	channelNums_.append(channelNum);
	latestFrameOffsetTimes_[channelNum] = -1;

	//Get Greatest Common Divisor of all sample resolutions
	//Loop through sample resolutions getting GCD of previous GCD and current sample resolution
	QHash<int, int>::iterator iter = sampResMap_.begin();
	int currGCD = iter.value();
	iter++;
	for(;iter !=sampResMap_.end();iter++)
	{
		if(currGCD > iter.value())
			currGCD = gcd(currGCD,iter.value());
		else
			currGCD = gcd(iter.value(),currGCD);
	}
	
	//Store the number of nidaq samples per data sample for each channel
	sampsPerReadMap_[channelNum] = 0;
	for(iter = sampResMap_.begin();iter != sampResMap_.end();iter++)
	{
		sampsPerReadMap_[iter.key()] = 4*iter.value()/currGCD;
	}

	//Set the minimum sample rate such that the samples read per ms is 1/4 of the GCD of all msPerSample values
	deviceSampRate_ = double(4000.0)/double(currGCD);
}

/*!	\brief Starts the DAQ collecting data.
 *
 *	This should be called as close to the point at which the data will actually be collected as possible.  Otherwise, data
 *	will pile up in the buffer. Also note that calling this clears out any existing data in the channel.
 *
 *	This function is called by updateDataBuffer()
 */
bool InputPort::start()
{
	if(!taskRunning_ && enabled_)
	{	
		Timestamper stamper;
		if(!startSampling())
			return false;

		//Set sampsCollected as if sampling started from the beginning of the Timestamper clock.
		sampsCollected_=stamper.stampSec()*deviceSampRate_;
		taskRunning_ = true;
		needsUpdate_ = false;
	}
	return true;

}

/*! The port starts sampling data as soon as the first updateDataBuffer() is called if the port is enabled.
 *	Disabling the port means that the port will start sampling from scratch the next time it is reenabled and
 *	updateDataBuffer() is called.
 */
void InputPort::enable(bool en)
{
	enabled_ = en;
	if(!enabled_ && taskRunning_)
	{
		stopSampling();

		//Clear all channelBuffer data vectors.
		QHash<int, QVector<double>>::iterator it = channelBuffers_.begin();
		for(it = channelBuffers_.begin();it != channelBuffers_.end();it++)
		{
			it->clear();
		}
		taskRunning_ = false;
		needsUpdate_ = false;
	}
}

/*! \brief Calls the empty updateDataBuffer() function to load data from the DAQ into channelBuffers_, then aligns
 *	timestamps in case of an imperfect DAQ clock.
 *	\details When this function loads data, it reads the time at which the data was actually read and calculates the time
 *	that the last data point came in assuming that each sample was read at exactly the time dictaded by a perfect clock for
 *	the set sample period.  If the calculated time is significantly lower than the actual time, a data point is duplicated
 *	to make up for it.  If the calculated time is significantly higher than the actual time, a data point is deleted to
 *	make up for it.  This function takes care of all of these issues.
 *
 *	The input currTimestamp is the last time for which data should be returned for the current frame.  Since there may have
 *	been some lag since the latest frame was displayed until this function was called, there may be some data points here
 *	that were read after the frame was over.  When we read this data though, we only want data that came in up to the time
 *	that the frame was presented.  This allows us to store data that came in too late for next frame and prepare only the
 *	data that occured during the previous frame.
 *	\note updateDataBuffer() doesn't do anything unless all input channel data has been read since the last call to this
 *	function.
 */
void InputPort::updateDataBuffer(double currTimestamp)
{
	//If the task isn't running yet, mark this time as the last syncTimstamp and start it.
	if(!taskRunning_)
	{
		lastSyncTimestamp_ = currTimestamp;
		
		bool started = start();
		IGNORED_PARAMETER(started);
		Q_ASSERT_X(started,"InputPort::updateDataBuffer","Port failed to start");
		return;
	}
	//Only allow update data buffer once all signal channels have looked at their data from the last update.
	if(!needsUpdate_)
		return;
	needsUpdate_ = false;

	double dataReadTime = updateDataBuffer();

	//If any data was left over last time this function was called (ie. it corresponded to times that were less than
	//	dataReadTime but greater than currTimestamp), it is added to the head of the list here.
	if(channelLeftOvers[channelNums_[0]].size())
	{
		QHash<int, QVector<double>>::iterator it;
		for(it = channelBuffers_.begin();it != channelBuffers_.end();it++)
		{
			int origSize = it.value().size();
			int extSize = channelLeftOvers[it.key()].size();
			it.value().resize(origSize + extSize);
			(*it) = channelLeftOvers[it.key()] << (*it);
			channelLeftOvers[it.key()].clear();
		}
	}

	int totalSampsRead = channelBuffers_[channelNums_[0]].size();

	//Throw out or duplicate readings to bring sample times into alignment.
	//	Since we are using at least 4 times the sample rate of the highest resolution channel, and the clock is good enough
	//	that we should never need to duplicate more than 1 sample to regain alignment, this is within any set resolution
	//	boundaries.
	double calculatedFinalReadTime = (sampsCollected_+totalSampsRead)/deviceSampRate_;
	double jitter = calculatedFinalReadTime-dataReadTime;
	int sampleListSizeChange = -jitter*deviceSampRate_;
	if(sampleListSizeChange)
	{
		QHash<int, QVector<double>>::iterator chanIter;
		int origSize;
		for(chanIter = channelBuffers_.begin();chanIter !=channelBuffers_.end();chanIter++)
		{
			origSize = chanIter.value().size();
			if(sampleListSizeChange + origSize < 0)
				sampleListSizeChange = origSize;
			chanIter.value().resize(origSize+sampleListSizeChange);
			if((sampleListSizeChange > 0) && origSize)
			{
				//If we increased the size of the vector, we should duplicate the last entry
				for(int i=origSize;i<origSize+sampleListSizeChange;i++)
				{
					chanIter.value()[i] = chanIter.value()[origSize-1];
				}
			}
		}
	}

	//We used the actual data read time reported from updateDataBuffer() to calculate jitter and synchronize accordingly.
	//	There may have been some delay between the passed in currTimestamp and the actual read time though.  The caller
	//	wants data that was read before currTimestamp.  For this reason, we remove the entries off the back of the
	//	channelBuffers_ vectors and save them for the next data read.
	int leftoverData = 0;
	calculatedFinalReadTime = (sampsCollected_+totalSampsRead+sampleListSizeChange)/deviceSampRate_;
	if(calculatedFinalReadTime > currTimestamp)
	{
		QHash<int, QVector<double>>::iterator it;
		leftoverData = 1+(calculatedFinalReadTime-currTimestamp)*deviceSampRate_;
		for(it = channelBuffers_.begin();it != channelBuffers_.end();it++)
		{
			channelLeftOvers[it.key()] << it->mid(it->size()-leftoverData,-1);
			it->resize(it->size()-leftoverData);
		}
	}

	//Update frame offset time values
	int sampsPerRead;
	int currPhase;
	foreach(int chanNum,sampsPerReadMap_.keys())
	{
		sampsPerRead = sampsPerReadMap_[chanNum];
		currPhase = channelPhaseMap_[chanNum];
		latestFrameOffsetTimes_[chanNum] = ((sampsCollected_+(sampsPerRead-currPhase))/deviceSampRate_)-lastSyncTimestamp_;  
	}

	//Update number of samples collected
	sampsCollected_ += (totalSampsRead+sampleListSizeChange-leftoverData);

	//Update last Sync Timestamp
	lastSyncTimestamp_ = currTimestamp;

	//Mark all channels as unpolled
	for(QHash<int, bool>::iterator it = polledDataMap_.begin();it != polledDataMap_.end();it++)
	{
		(*it) = false;
	}
}

/*! \brief Returns true if there is new data available on this InputPort.
 */
bool InputPort::hasNewData(int channelNum)
{
	return channelBuffers_[channelNum].size() > 0;
}

/*! \brief Returns a vector of data samples for the input channel number.
 *	\note A side effect of this function is that it clears out the data buffer for the input channelNum.
 */
QVector<double> InputPort::getData(int channelNum)
{
	Q_ASSERT_X(sampResMap_.contains(channelNum),"InputPort::getData","Data was requested for a channel that doesn't exist");

	int sampsPerRead = sampsPerReadMap_[channelNum];
	int currPhase = channelPhaseMap_[channelNum];
	QVector<double>* currList = &channelBuffers_[channelNum];

	//Create the return array with the correct size
	QVector<double> returnVal((currList->size()+currPhase)/sampsPerRead);

	//Put every "sampsPerRead"th value from the daq data into the return array taking into account last time's
	//	remaining phase.
	int i=0, j=0;
	for(;i<currList->size();i++)
	{
		currPhase++;
		if(currPhase == sampsPerRead)
		{
			returnVal[j++] = (*currList)[i];
			currPhase = 0;
		}
	}
	channelPhaseMap_[channelNum] = currPhase;
	
	//Mark this channel's data as polled.
	polledDataMap_[channelNum] = true;

	//Clear buffer for next data update
	channelBuffers_[channelNum].clear();

	//If any data channel hasn't been polled yet, we're done.
	foreach(bool polled,polledDataMap_)
	{
		if(!polled)
			return returnVal;
	}
	//All data channels have been polled, we're ready for the next nidaq card update.
	needsUpdate_ = true;

	return returnVal;
}

/*!	\brief Returns the offset from the time that the previous frame occured to the time that the first sample on the input
 *	channel was read.
 */
double InputPort::getFrameToSampleOffset(int channelNum)
{
	return latestFrameOffsetTimes_[channelNum];
}