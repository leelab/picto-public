#include "SignalChannel.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new SignalChannel with the input name and InputPort.
 *	\details When using this constructor a default value of 1 reading per ms is used.
 *	\note Multiple SignalChannels can share  a single InputPort.  The InputPort represents a DAQ device, for example,
 *	whereas the SignalChannel represents one logical signal such as "Position."
 */
SignalChannel::SignalChannel(QString name,QSharedPointer<InputPort> port)
{
	name_ = name;
	port_ = port;
	setSampleResolution(1);
}

/*! \brief Constructs a new SignalChannel with the input name, InputPort and msPerSample sample period.
 *	\note Multiple SignalChannels can share  a single InputPort.  The InputPort represents a DAQ device, for example,
 *	whereas the SignalChannel represents one logical signal such as "Position."
 */
SignalChannel::SignalChannel(QString name, int msPerSample, QSharedPointer<InputPort> port)
{
	name_ = name.toLower();
	port_ = port;
	setSampleResolution(msPerSample);
}

/*! \brief Adds a subchannel for this SignalChannel.
 *	\details Subchannels allow a signal to be broken into its sub components.  The position signal for example includes two
 *	sub channels, x and y.  Each subchannel should be provided with a name that is unique to this SignalChannel and a
 *	channelIndex that is unique to the InputPort.
 */
void SignalChannel::addSubchannel(QString subchannelName, int channelIndex)
{
	QVector<double> data;
	if(!rawDataBuffer_.contains(subchannelName))
	{
		rawDataBuffer_[subchannelName] = data;
		rawDataLastValue_[subchannelName] = 0.0;
	}

	if(port_)
	{
		port_->addInputChannel(channelIndex,msPerSample_);
		if(!channelIndexMap_.contains(subchannelName))
		{
			channelIndexMap_[subchannelName] = channelIndex;
		}
	}

}

/*! \brief Sets the resolution at which this channel will be sampled.
 *	\details All sub channels of a given signal channel will be sampled at the same rate.
 */
void SignalChannel::setSampleResolution(int msPerSample)
{
	Q_ASSERT(msPerSample != 0);
	msPerSample_ = msPerSample;
}

/*!	\brief Returns the offset from the time that the previous frame occured to the time that the first sample appears on
 *	the SignalChannel.
 *	\note This will always be less than the sample rate.
 *	\sa InputPort::getFrameToSampleOffset()
 */
double SignalChannel::latestUpdateEventOffset()
{
	if(!port_)
		return 0;
	return port_->getFrameToSampleOffset(channelIndexMap_.begin().value());
}

/*! \brief Gets the most recent value from the subchannel.
 *	\details This function does not cause data to be deleted.  The data returned by this function will still be returned in
 *	the next call to getValues().
 */
double SignalChannel::peekValue(QString subchannel)
{
	getDataFromPort();

	if(!rawDataBuffer_.contains(subchannel))
		return 0.0;

	//Update the rawDataLastValue_ map with this subchannel and a shearAsFuncOf channel
	if(!rawDataBuffer_.value(subchannel).isEmpty())
	{
		rawDataLastValue_[subchannel] = rawDataBuffer_.value(subchannel).last();
	}

	Q_ASSERT(rawDataLastValue_.contains(subchannel));

	return rawDataLastValue_.value(subchannel);
}

/*! \brief Clears the latest data that was read in from the InputPort.
 */
void SignalChannel::clearValues()
{
	//clear out the raw data
	QMap<QString, QVector<double> >::iterator x = rawDataBuffer_.begin();

	while(x != rawDataBuffer_.end())
	{
		//Update the last raw value that was read.
		if(!x.value().isEmpty())
			rawDataLastValue_[x.key()] = x.value().last();
		x.value().clear();
		x++;
	}
}

/*! \brief Starts data capture for this SignalChannel.
 */
bool SignalChannel::start()
{
	if(!port_)
		return true;
	port_->enable(true);
	return true;
}

/*! \brief Stops data capture for this SignalChannel and clears out any remaining data in buffers.
 */
bool SignalChannel::stop()
{
	if(!port_)
		return true;
	port_->enable(false);
	clearValues();
	return true;
}

/*! \brief Returns a map of data sample vectors indexed by sub-channel name.
 *	\note This function calls getRawValues() which has a side effect in that it clears out all read data such that the
 *	next call to getValues() will return only the values read since the function was last called.
 */
QMap<QString, QVector<double> > SignalChannel::getValues()
{
	return getRawValues();
}

/*! \brief Loads data into the underlying InputPort so that it will be available for the next call to getValues().
 *	\details This function is pretty much just a wrapper for InputPort::updateDataBuffer()
 */
void SignalChannel::updateData(double currentTime)
{
	if(port_)
		port_->updateDataBuffer(currentTime);
}

/*! \brief Gets data for this SignalChannel from the InputPort and returns it.
 *	\note A side effect of this function is that it clears out all read data such that the next call to getRawValues() will
 *	return only the values read since the function was last called.
 */
QMap<QString, QVector<double> > SignalChannel::getRawValues()
{
	getDataFromPort();

	QMap<QString, QVector<double> > dataBuffer = rawDataBuffer_;
	
	//clear out the raw data
	QMap<QString, QVector<double> >::iterator x = rawDataBuffer_.begin();

	while(x != rawDataBuffer_.end())
	{
		//Update the last raw value that was read.
		if(!x.value().isEmpty())
			rawDataLastValue_[x.key()] = x.value().last();
		x.value().clear();
		x++;
	}

	return dataBuffer;
}

/*! \brief Adds a single input value to the input subchannel from an outside source.
 *	\details This is useful for slave situations in which the data is coming in from a master and not from an underlying
 *	InputPort.
 *	\note It would probably be more logically consistent if we created a SlaveInputPort that recieved data from the master
 *	or possibly a SlaveSignalChannel class that would reimplement some of this class's functions since master data is
 *	prescaled.  For now, this is working even though it isn't too clean.
 */
void SignalChannel::insertValue(QString subchannel, double val)
{
	//When this is called, the passed in value is immediately added to the rawDataBuffer_.  Since real data is only added
	//	to the rawDataBuffer_ when updateDataBuffer is called, the inserted data is slightly out of order.  This shouldn't
	//	be a big deal, since it is unlikely that we'll be inserting values at the same time as we're collecting real data.
	//	Should this become an issue, we'll want to add timestamps.
	
	Q_ASSERT(rawDataBuffer_.contains(subchannel));

	rawDataBuffer_[subchannel].append(val);
	rawDataLastValue_[subchannel] = val;
	
}

/*! \brief Adds data values to the input sub-channel from an outside source.
 *	\details This is useful for slave situations in which the data is coming in from a master and not from an underlying
 *	InputPort.
 *	\note It would probably be more logically consistent if we created a SlaveInputPort that recieved data from the master
 *	or possibly a SlaveSignalChannel class that would reimplement some of this class's functions since master data is
 *	prescaled.  For now, this is working even though it isn't too clean.
 */
void SignalChannel::insertValues(QString subchannel, QVector<double> vals)
{
	if(rawDataBuffer_.contains(subchannel))
	{
		rawDataBuffer_[subchannel] << vals;
		if(rawDataBuffer_[subchannel].size())
			rawDataLastValue_[subchannel] = vals.last();
	}
}

/*! \brief Copies data in from the InputPort to this SignalChannel for all sub-channels.
 *	\details Internally, this calls InputPort::getData()
 */
void SignalChannel::getDataFromPort()
{
	if(!port_)
		return;
	QMap<QString, int>::iterator it;
	for(it = channelIndexMap_.begin();it != channelIndexMap_.end();it++)
	{
		rawDataBuffer_[it.key()] << port_->getData(it.value());
		if(rawDataBuffer_[it.key()].size())
			rawDataLastValue_[it.key()] = rawDataBuffer_[it.key()].last();
	}

}

}; //namespace Picto
