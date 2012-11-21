#include "SignalChannel.h"
#include "../memleakdetect.h"

namespace Picto {

SignalChannel::SignalChannel(QString name,QSharedPointer<InputPort> port)
{
	name_ = name;
	port_ = port;
	setSampleResolution(1);
	useScaleFactors_ = true;
}

SignalChannel::SignalChannel(QString name, int msPerSample, QSharedPointer<InputPort> port)
{
	name_ = name.toLower();
	port_ = port;
	setSampleResolution(msPerSample);
	useScaleFactors_ = true;
}

void SignalChannel::addSubchannel(QString subchannelName, int channelIndex)
{
	QVector<double> data;
	if(!rawDataBuffer_.contains(subchannelName))
	{
		rawDataBuffer_[subchannelName] = data;
		rawDataLastValue_[subchannelName] = 0.0;
	}
	
	if(!scaleFactorsMap_.contains(subchannelName))
	{
		scaleFactorsMap_[subchannelName].scaleB = 1;
		scaleFactorsMap_[subchannelName].scaleA = 0;
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

void SignalChannel::setSampleResolution(int msPerSample)
{
	Q_ASSERT(msPerSample != 0);
	msPerSample_ = msPerSample;
}

void SignalChannel::setCalibrationCoefficientsFromRange(QString subchannel, double minRawValue, double maxRawValue, double minScaledValue, double maxScaledValue)
{
	if(!useScaleFactors_)
		return;
	//set the scaling values (we're assuming a linear scaling with 
	//y = A + Bx
	scaleFactorsMap_[subchannel].scaleB = (maxScaledValue-minScaledValue)/(maxRawValue-minRawValue);
	scaleFactorsMap_[subchannel].scaleA = maxScaledValue-scaleFactorsMap_[subchannel].scaleB*maxRawValue;
	scaleFactorsMap_[subchannel].centerVal = (maxScaledValue-minScaledValue)/2.0;
}

void SignalChannel::setCalibrationCoefficients(QString subchannel, double gain, int offset, double scaledCenterValue)
{
	if(!useScaleFactors_)
		return;
	//set the scaling values (we're assuming a linear scaling with 
	//y = A + Bx
	scaleFactorsMap_[subchannel].scaleB = gain;
	scaleFactorsMap_[subchannel].scaleA = offset;
	scaleFactorsMap_[subchannel].centerVal = scaledCenterValue;
}

/*!
 *	Each value is sheared after applying scale factors.  The new value x = x + shearFactor*asFuncOfSubChannel_Value
 */
void SignalChannel::setShear(QString subchannel, QString asFuncOfSubChannel, double shearFactor)
{
	if(!useScaleFactors_)
		return;
	Q_ASSERT(scaleFactorsMap_.contains(asFuncOfSubChannel));
	Q_ASSERT(scaleFactorsMap_.value(asFuncOfSubChannel).shearAsFuncOf.isEmpty());
	scaleFactorsMap_[subchannel].shearAsFuncOf = asFuncOfSubChannel;
	scaleFactorsMap_[subchannel].shearFactor = shearFactor;
}

double SignalChannel::latestUpdateEventOffset()
{
	if(!port_)
		return 0;
	return port_->getFrameToSampleOffset(channelIndexMap_.begin().value());
}

//! Grabs the most recent value from the subchannel, scales it, and returns it
double SignalChannel::peekValue(QString subchannel)
{
	getDataFromPort();

	if(!rawDataBuffer_.contains(subchannel))
		return 0.0;
	QString shearAsFuncOf = scaleFactorsMap_.value(subchannel).shearAsFuncOf;
	Q_ASSERT(shearAsFuncOf.isEmpty() || rawDataBuffer_.contains(shearAsFuncOf));
	//Update the rawDataLastValue_ map with this subchannel and a shearAsFuncOf channel
	if(!rawDataBuffer_.value(subchannel).isEmpty())
	{
		rawDataLastValue_[subchannel] = rawDataBuffer_.value(subchannel).last();
	}
	if(!shearAsFuncOf.isEmpty() && !rawDataBuffer_.value(shearAsFuncOf).isEmpty())
	{
		rawDataLastValue_[shearAsFuncOf] = rawDataBuffer_.value(shearAsFuncOf).last();
	}
	Q_ASSERT(rawDataLastValue_.contains(subchannel));
	Q_ASSERT(shearAsFuncOf.isEmpty() || rawDataLastValue_.contains(subchannel));

	//Get the lastRawDataValue, scale and shear it.
	double rawValue = rawDataLastValue_.value(subchannel);
	double scaledValue = scaleFactorsMap_.value(subchannel).scaleA + 
				scaleFactorsMap_.value(subchannel).scaleB * rawValue;
	if(!shearAsFuncOf.isEmpty())
	{
		double otherRawValue = rawDataLastValue_.value(shearAsFuncOf);
		double otherScaledValue = scaleFactorsMap_.value(shearAsFuncOf).scaleA + 
			scaleFactorsMap_.value(shearAsFuncOf).scaleB * otherRawValue;
		scaledValue = scaledValue + scaleFactorsMap_.value(subchannel).shearFactor * (otherScaledValue - scaleFactorsMap_.value(shearAsFuncOf).centerVal);
	}
	//qDebug(QString("%1: Last: %2 Scaled: %3").arg(rawDataLastValue_[subchannel]).arg(subchannel).arg(scaledValue).toAscii());
	return scaledValue;
}

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

bool SignalChannel::start()
{
	if(!port_)
		return true;
	port_->enable(true);
	return true;
}

bool SignalChannel::stop()
{
	if(!port_)
		return true;
	port_->enable(false);
	clearValues();
	return true;
}

QMap<QString, QVector<double> > SignalChannel::getValues()
{
	QMap<QString, QVector<double> > dataBuffer = getRawValues();

	//scale the values
	QMap<QString, QVector<double> >::iterator x = dataBuffer.begin();

	while(x != dataBuffer.end())
	{
		for(QVector<double>::iterator subChanValIter = x.value().begin(); 
			subChanValIter != x.value().end(); 
			subChanValIter++)
		{
			double scaledValue = scaleFactorsMap_.value(x.key()).scaleA + 
				scaleFactorsMap_.value(x.key()).scaleB * (*subChanValIter);
			(*subChanValIter) = scaledValue;
		}
		x++;
	}

	//Add shear
	x = dataBuffer.begin();
	while(x != dataBuffer.end())
	{
		QString shearAsFuncOf = scaleFactorsMap_.value(x.key()).shearAsFuncOf;
		if(shearAsFuncOf.isEmpty())
		{
			x++;
			continue;
		}

		QVector<double>::iterator funcOfIterator = dataBuffer.find(shearAsFuncOf).value().begin();
		for(QVector<double>::iterator subChanValIter = x.value().begin(); 
			subChanValIter != x.value().end(); 
			subChanValIter++,funcOfIterator++)
		{
			double shearedVal = (*subChanValIter) + 
								scaleFactorsMap_.value(x.key()).shearFactor * ((*funcOfIterator)-scaleFactorsMap_.value(shearAsFuncOf).centerVal);
			(*subChanValIter) = shearedVal;
		}
		x++;
	}

	return dataBuffer;
}

void SignalChannel::updateData(double currentTime)
{
	if(port_)
		port_->updateDataBuffer(currentTime);
}

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

/*! \brief Gets Signal Channel data in the form of a BehavioralDataUnitPackage.
 *	This function internally calls getValues and therefore has the effect of clearing
 *	the value buffer.
 */
QSharedPointer<BehavioralDataUnitPackage> SignalChannel::getDataPackage()
{
	QSharedPointer<BehavioralDataUnitPackage> returnVal(new BehavioralDataUnitPackage());
	returnVal->setChannel(getName());
	returnVal->setResolution(msPerSample_);
	returnVal->addData(getValues(),latestUpdateEventOffset());
	if(returnVal->length())
		return returnVal;
	return QSharedPointer<BehavioralDataUnitPackage>();
}

//When this is called, the passed in value is immediately added to the 
//rawDataBuffer_.  Since real data is only added to the rawDataBuffer_
//when updateDataBuffer is called, the inserted data is slightly out of
//order.  This shouldn't be a big deal, since it is unlikely that we'll be 
//inserting values at the same time as we're collecting real data.
//Should this become an issue, we'll want to add timestamps.
void SignalChannel::insertValue(QString subchannel, double val)
{
	Q_ASSERT(rawDataBuffer_.contains(subchannel));
	rawDataBuffer_[subchannel].append(val);
	rawDataLastValue_[subchannel] = val;
	
}

void SignalChannel::insertValues(QString subchannel, QVector<double> vals)
{
	if(rawDataBuffer_.contains(subchannel))
	{
		rawDataBuffer_[subchannel] << vals;
		if(rawDataBuffer_[subchannel].size())
			rawDataLastValue_[subchannel] = vals.last();
	}
}

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
