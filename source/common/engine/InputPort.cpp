#include "InputPort.h"

#include "../../common/timing/Timestamper.h"
#include "../../common/memleakdetect.h"

using namespace Picto;


//Recursive Greatest Common Devisor function for use in calculating the nidaqSampleRate.
//Pretty cool right?!  See http://www.phailed.me/2011/02/greatest-common-denominator/
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

void InputPort::addInputChannel(int channelNum, int msPerSample)
{
	Q_ASSERT(!sampResMap_.contains(channelNum));
	sampResMap_[channelNum] = msPerSample;
	polledDataMap_[channelNum] = true;	//Since this channel was just added, its as if it was just polled.
	channelPhaseMap_[channelNum] = 0;
	channelNums_.append(channelNum);
	latestFrameOffsetTimes_[channelNum] = -1;

	//Get Greatest Common Devisor of all sample resolutions
	//Loop through sample resolutions getting GCD of previous GCD
	//and current sample resolution
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
 *	This should be called as close to the point at which the data will actually
 *	be collected as possible.  Otherwise, data will pile up in the buffer.
 *	Also note that calling this clears out any existing data in the channel.
 */
bool InputPort::start()
{
	if(!taskRunning_ && enabled_)
	{	
		Timestamper stamper;
		if(!startSampling())
			return false;

		//Set sampsCollected as if sampling started from
		//the beginning of the Timestamper clock.
		sampsCollected_=stamper.stampSec()*deviceSampRate_;
		taskRunning_ = true;
		needsUpdate_ = false;
	}
	return true;

}

/*! The port starts sampling data as soon as the first updateDataBuffer() is
 * called if the port is enabled.  disabling the port means that the port will
 * start sampling from scratch the next time it is reenabled and 
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

void InputPort::updateDataBuffer(double currTimestamp)
{
	//If the task isn't running yet, mark this time as the last syncTimstamp and start it.
	if(!taskRunning_)
	{
		lastSyncTimestamp_ = currTimestamp;
		
		bool started = start();
		Q_ASSERT_X(started,"InputPort::updateDataBuffer","Port failed to start");
		return;
	}
	//Only allow update data buffer once all signal channels have looked at their data from the last update.
	if(!needsUpdate_)
		return;
	needsUpdate_ = false;

	double dataReadTime = updateDataBuffer();

	//If any data was left over last time this function was called (ie. it corresponded 
	//to times that were less than dataReadTime but greater than currTimestamp, it is
	//added to the head of the list here.
	if(channelLeftOvers[channelNums_[0]].size())
	{
		int lastTimesLeftovers = channelLeftOvers[channelNums_[0]].size();
		QHash<int, QVector<double>>::iterator it;
		for(it = channelBuffers_.begin();it != channelBuffers_.end();it++)
		{
			(*it) = channelLeftOvers[it.key()] << (*it);
			channelLeftOvers[it.key()].clear();
		}
	}

	int totalSampsRead = channelBuffers_[channelNums_[0]].size();

	//Throw out or duplicate readings to bring sample times into alignment
	//Since we are using at least 4 times the sample rate of the highest
	//resolution channel, and the clock is good enough that we should never 
	//need to duplicate more than 1 sample to regain alignment, this is 
	//within any set resolution boundaries.
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

	//We used the actual data read time reported from updateDataBuffer() to calculate
	//jitter and synchronize accordingly.  There may have been some delay between the 
	//passed in currTimestamp and the actual read time though.  The caller wants data
	//that was read before currTimestamp.  For this reason, we remove the entries off
	//the back of the channelBuffers_ vectors and save them for the next data read.
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

bool InputPort::hasNewData(int channelNum)
{
	return channelBuffers_[channelNum].size() > 0;
}

QVector<double> InputPort::getData(int channelNum)
{
	Q_ASSERT_X(sampResMap_.contains(channelNum),"InputPort::getData","Data was requested for a channel that doesn't exist");

	int sampsPerRead = sampsPerReadMap_[channelNum];
	int currPhase = channelPhaseMap_[channelNum];
	QVector<double>* currList = &channelBuffers_[channelNum];
	//Create the return array with the correct size
	int returnArraySize = (currList->size()+currPhase)/sampsPerRead;
	QVector<double> returnVal((currList->size()+currPhase)/sampsPerRead);

	//Put every "sampsPerReadth" value from the nidaq data into the return array taking
	//into account last time's remaining phase.
	int i=0, j=0;
	for(;i<currList->size();i++)
	{
		currPhase++;
		if(currPhase == sampsPerRead)
		{
			double currVal = (*currList)[i];
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

double InputPort::getFrameToSampleOffset(int channelNum)
{
	return latestFrameOffsetTimes_[channelNum];
}