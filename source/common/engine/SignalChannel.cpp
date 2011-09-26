#include "SignalChannel.h"

namespace Picto {

SignalChannel::SignalChannel()
{
	sampleRate_ = 1;
	useScaleFactors_ = true;
}
SignalChannel::SignalChannel(int sampsPerSec)
{
	sampleRate_ = sampsPerSec;
	useScaleFactors_ = true;
}


void SignalChannel::setsampleRate_(int sampsPerSec)
{
	sampleRate_ = sampsPerSec;
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

void SignalChannel::addSubchannel(QString subchannelName)
{
	QList<double> data;
	if(!rawDataBuffer_.contains(subchannelName))
		rawDataBuffer_[subchannelName] = data;
	
	if(!scaleFactorsMap_.contains(subchannelName))
	{
		scaleFactorsMap_[subchannelName].scaleB = 1;
		scaleFactorsMap_[subchannelName].scaleA = 0;
	}

}

//! Grabs the most recent value from the subchannel, scales it, and returns it
double SignalChannel::peekValue(QString subchannel)
{
	updateDataBuffer();

	if(rawDataBuffer_.isEmpty())
		return 0.0;
	if(rawDataBuffer_.value(subchannel).isEmpty())
		return 0.0;

	double rawValue = rawDataBuffer_.value(subchannel).last();
	double scaledValue = scaleFactorsMap_.value(subchannel).scaleA + 
				scaleFactorsMap_.value(subchannel).scaleB * rawValue;
	QString scaleAsFuncOf = scaleFactorsMap_.value(subchannel).shearAsFuncOf;
	if(!scaleAsFuncOf.isEmpty())
	{
		double otherRawValue = rawDataBuffer_.value(scaleAsFuncOf).last();
		double otherScaledValue = scaleFactorsMap_.value(scaleAsFuncOf).scaleA + 
			scaleFactorsMap_.value(scaleAsFuncOf).scaleB * otherRawValue;
		scaledValue = scaledValue + scaleFactorsMap_.value(subchannel).shearFactor * (otherScaledValue - scaleFactorsMap_.value(scaleAsFuncOf).centerVal);
	}
	return scaledValue;
}


QMap<QString, QList<double> > SignalChannel::getValues()
{
	QMap<QString, QList<double> > dataBuffer = getRawValues();

	//scale the values
	QMap<QString, QList<double> >::iterator x = dataBuffer.begin();

	while(x != dataBuffer.end())
	{
		for(QList<double>::iterator subChanValIter = x.value().begin(); 
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

		QList<double>::iterator funcOfIterator = dataBuffer.find(shearAsFuncOf).value().begin();
		for(QList<double>::iterator subChanValIter = x.value().begin(); 
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

QMap<QString, QList<double> > SignalChannel::getRawValues()
{
	updateDataBuffer();

	QMap<QString, QList<double> > dataBuffer = rawDataBuffer_;
	
	//clear out the raw data
	QMap<QString, QList<double> >::iterator x = rawDataBuffer_.begin();

	while(x != rawDataBuffer_.end())
	{
		x.value().clear();
		x++;
	}

	return dataBuffer;
}



//When this is called, the passed in value is immediately added to the 
//rawDataBuffer_.  Since real data is only added to the rawDataBuffer_
//when updateDataBuffer is called, the inserted data is slightly out of
//order.  This shouldn't be a big deal, since it is unlikely that we'll be 
//inserting values at the same time as we're collecting real data.
//Should this become an issue, we'll want to add timestamps.
void SignalChannel::insertValue(QString subchannel, double val)
{
	if(rawDataBuffer_.contains(subchannel))
	{
		rawDataBuffer_[subchannel].append(val);
	}
}

void SignalChannel::insertValues(QString subchannel, QList<double> vals)
{
	if(rawDataBuffer_.contains(subchannel))
	{
		rawDataBuffer_[subchannel].append(vals);
	}
}


}; //namespace Picto
