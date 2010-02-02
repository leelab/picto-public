#include "SignalChannel.h"

namespace Picto {

SignalChannel::SignalChannel()
{
	sampleRate = 1;
}
SignalChannel::SignalChannel(int sampsPerSec)
{
	sampleRate = sampsPerSec;
}


void SignalChannel::setSampleRate(int sampsPerSec)
{
	sampleRate = sampsPerSec;
}

void SignalChannel::setCalibrationRange(QString subchannel, double minRawValue, double maxRawValue, double minScaledValue, double maxScaledValue)
{
	//set the scaling values (we're assuming a linear scaling with 
	//y = A + Bx
	scaleFactorsMap[subchannel].scaleB = (maxScaledValue-minScaledValue)/(maxRawValue-minRawValue);
	scaleFactorsMap[subchannel].scaleA = maxScaledValue-scaleFactorsMap[subchannel].scaleB*maxRawValue;
}

//! Set the scaling coefficients directly (A + Bx)

/*!
 *	Each value is linearly scaled using the formula A + Bx.  This function
 *	sets the scaling coefficients directly.
 */
void SignalChannel::setCalibrationCoefficients(QString subchannel, double A, double B)
{
	scaleFactorsMap[subchannel].scaleA = A;
	scaleFactorsMap[subchannel].scaleB = B;
}


void SignalChannel::addSubchannel(QString subchannelName)
{
	QList<double> data;
	if(!rawDataBuffer.contains(subchannelName))
		rawDataBuffer[subchannelName] = data;
	
	if(!scaleFactorsMap.contains(subchannelName))
	{
		scaleFactorsMap[subchannelName].scaleB = 1;
		scaleFactorsMap[subchannelName].scaleA = 0;
	}

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
			double scaledValue = scaleFactorsMap.value(x.key()).scaleA + 
				scaleFactorsMap.value(x.key()).scaleB * x.value().at(y);
			x.value().replace(y,scaledValue);
		}
		x++;
	}

	return dataBuffer;
}

QMap<QString, QList<double> > SignalChannel::getRawValues()
{
	updateDataBuffer();

	QMap<QString, QList<double> > dataBuffer = rawDataBuffer;
	
	//clear out the raw data
	QMap<QString, QList<double> >::iterator x = rawDataBuffer.begin();

	while(x != rawDataBuffer.end())
	{
		x.value().clear();
		x++;
	}

	return dataBuffer;
}



//When this is called, the passed in value is immediately added to the 
//rawDataBuffer.  Since real data is only added to the rawDataBuffer
//when updateDataBuffer is called, the inserted data is slightly out of
//order.  This shouldn't be a big deal, since it is unlikely that we'll be 
//inserting values at the same time as we're collecting real data.
//Should this become an issue, we'll want to add timestamps.
void SignalChannel::insertValue(QString subchannel, double val)
{
	if(rawDataBuffer.contains(subchannel))
	{
		rawDataBuffer[subchannel].append(val);
	}
}

void SignalChannel::insertValues(QString subchannel, QList<double> vals)
{
	if(rawDataBuffer.contains(subchannel))
	{
		rawDataBuffer[subchannel].append(vals);
	}
}


}; //namespace Picto
