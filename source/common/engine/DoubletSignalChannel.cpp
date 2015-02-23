#include "DoubletSignalChannel.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new DoubletSignalChannel with the input name and InputPort.
 *	\details When using this constructor a default value of 1 reading per ms is used.
 *	\note Multiple SignalChannels can share  a single InputPort.  The InputPort represents
 *	a DAQ device, for example, whereas the SignalChannel represents one logical signal such
 *	as "Position."
*/
DoubletSignalChannel::DoubletSignalChannel(QString name, QSharedPointer<InputPort> port)
	: SignalChannel(name, port)
{
	useScaleFactors_ = true;
}

/*! \brief Constructs a new DoubletSignalChannel with the input name, InputPort and msPerSample sample period.
 *	\note Multiple SignalChannels can share  a single InputPort.  The InputPort represents
 *	a DAQ device, for example, whereas the SignalChannel represents one logical signal such
 *	as "Position."
*/
DoubletSignalChannel::DoubletSignalChannel(QString name, int msPerSample, QSharedPointer<InputPort> port)
	: SignalChannel(name, msPerSample, port)
{
	useScaleFactors_ = true;
}

/*! \brief Adds a subchannel for this SignalChannel.
*	\details Subchannels allow a signal to be broken into its sub components.  The position
*	signal for example includes two sub channels, x and y.  Each subchannel should be provided
*	with a name that is unique to this SignalChannel and a channelIndex that is unique to the InputPort.
*/
void DoubletSignalChannel::addSubchannel(QString subchannelName, int channelIndex)
{
	SignalChannel::addSubchannel(subchannelName, channelIndex);

	if (!scaleFactorsMap_.contains(subchannelName))
	{
		scaleFactorsMap_[subchannelName].scaleB = 1;
		scaleFactorsMap_[subchannelName].scaleA = 0;
	}
}

/*! \brief This is a convenience function allowing calibration coefficients to be calculated from boundary conditions rather than
 *	being set explicitly.
 *	\details Input the min and max raw InputPort values and the min and max scaled values that should be output from this
 *	SignalChannel.  The function will calculate A and B in output = A + B*Input such that the min raw input will produce
 *	the min scaled output and the max raw input will produce the max raw output.
 */
void DoubletSignalChannel::setCalibrationCoefficientsFromRange(QString subchannel, double minRawValue, double maxRawValue, double minScaledValue, double maxScaledValue)
{
	if(!useScaleFactors_)
		return;
	//set the scaling values (we're assuming a linear scaling with 
	//y = A + Bx
	scaleFactorsMap_[subchannel].scaleB = (maxScaledValue-minScaledValue)/(maxRawValue-minRawValue);
	scaleFactorsMap_[subchannel].scaleA = maxScaledValue-scaleFactorsMap_[subchannel].scaleB*maxRawValue;
	scaleFactorsMap_[subchannel].centerVal = (maxScaledValue-minScaledValue)/2.0;
}

/*! \brief Sets the calibration coefficients that will be used to scale/translate raw data from the InputPort into clean SignalChannel output.
 *	\details The value will be scaled like \code output=InputPortData*gain + offset \endcode
 *	\details scaledCenterValue is used in case another sub-channel shears with respect to this channel.  The idea is that
 *	that sub-channels shear coefficient will be multiplied by this sub-channel's scaled offset from its center value and the
 *	result of that multiplication will be added to the other sub-channel value
 */
void DoubletSignalChannel::setCalibrationCoefficients(QString subchannel, double gain, int offset, double scaledCenterValue)
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
 *	\brief Sets up shearing dependency for the input subchannel as a function of the input asFuncOfSubChannel with the input
 *	shearFactor.
 *	\details Note that values are sheared after applying regular linear scale factors such that the new value 
 *	\code scaledShearedOutput = scaledOutput + shearFactor*asFuncOfSubChannel.value \endcode
 */
void DoubletSignalChannel::setShear(QString subchannel, QString asFuncOfSubChannel, double shearFactor)
{
	if(!useScaleFactors_)
		return;
	Q_ASSERT(scaleFactorsMap_.contains(asFuncOfSubChannel));
	Q_ASSERT(scaleFactorsMap_.value(asFuncOfSubChannel).shearAsFuncOf.isEmpty());
	scaleFactorsMap_[subchannel].shearAsFuncOf = asFuncOfSubChannel;
	scaleFactorsMap_[subchannel].shearFactor = shearFactor;
}

/*! \brief Gets the most recent value from the subchannel, scales it, and returns it.
 *	\details This function does not cause data to be deleted.  The data returned by this
 *	function will still be returned in the next call to getValues().
 */
double DoubletSignalChannel::peekValue(QString subchannel)
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
	//qDebug(QString("%1: Last: %2 Scaled: %3").arg(rawDataLastValue_[subchannel]).arg(subchannel).arg(scaledValue).toLatin1());
	return scaledValue;
}

/*! \brief Returns a map of data sample vectors indexed by sub-channel name.
 *	\details All returned data is scaled and sheared.
 *	\note This function calls getRawValues() which has a side effect in that it clears out all read data such that the
 *	next call to getValues() will return only the values read since the function was last called.
 */
QMap<QString, QVector<double> > DoubletSignalChannel::getValues()
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

/*! \brief Gets Signal Channel data in the form of a BehavioralDataUnitPackage.
 *	\note This function internally calls getValues() and therefore has the side effect of clearing
 *	the value buffer such that the next call to this function will return entirely new data
 *	starting from when the function was last called.
 */
QSharedPointer<BehavioralDataUnitPackage> DoubletSignalChannel::getDataPackage()
{
	QSharedPointer<BehavioralDataUnitPackage> returnVal(new BehavioralDataUnitPackage());
	returnVal->setChannel(getName());
	returnVal->setResolution(msPerSample_);
	returnVal->addData(getValues(),latestUpdateEventOffset());
	if(returnVal->length())
		return returnVal;
	return QSharedPointer<BehavioralDataUnitPackage>();
}

}; //namespace Picto
