#include "SignalChannel.h"

namespace Picto {

SignalChannel::SignalChannel()
{
	sampleRate_ = 1;
}
SignalChannel::SignalChannel(int sampsPerSec)
{
	sampleRate_ = sampsPerSec;
}


void SignalChannel::setsampleRate_(int sampsPerSec)
{
	sampleRate_ = sampsPerSec;
}

void SignalChannel::setCalibrationRange(QString subchannel, double minRawValue, double maxRawValue, double minScaledValue, double maxScaledValue)
{
	//set the scaling values (we're assuming a linear scaling with 
	//y = A + Bx
	scaleFactorsMap_[subchannel].scaleB = (maxScaledValue-minScaledValue)/(maxRawValue-minRawValue);
	scaleFactorsMap_[subchannel].scaleA = maxScaledValue-scaleFactorsMap_[subchannel].scaleB*maxRawValue;
}

//! Set the scaling coefficients directly (A + Bx)

/*!
 *	Each value is linearly scaled using the formula A + Bx.  This function
 *	sets the scaling coefficients directly.
 */
void SignalChannel::setCalibrationCoefficients(QString subchannel, double A, double B)
{
	scaleFactorsMap_[subchannel].scaleA = A;
	scaleFactorsMap_[subchannel].scaleB = B;
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
	double rawValue = rawDataBuffer_.value(subchannel).last();
	double scaledValue = scaleFactorsMap_.value(subchannel).scaleA + 
				scaleFactorsMap_.value(subchannel).scaleB * rawValue;
	return scaledValue;
}


QMap<QString, QList<double> > SignalChannel::getValues()
{
	QMap<QString, QList<double> > dataBuffer = getRawValues();

	//scale the values
	QMap<QString, QList<double> >::iterator x = dataBuffer.begin();

	while(x != dataBuffer.end())
	{
		for(int y=0; y<x.value().size(); y++)
		{
			double scaledValue = scaleFactorsMap_.value(x.key()).scaleA + 
				scaleFactorsMap_.value(x.key()).scaleB * x.value().at(y);
			x.value().replace(y,scaledValue);
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
