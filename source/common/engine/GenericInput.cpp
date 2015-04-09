#include "GenericInput.h"

#include "../memleakdetect.h"

using namespace Picto;

//! The High voltage in Analog Input setup
#define ANALOG_VOLTAGE_MAX 10.0

//! The Low voltage in Analog Input setup
#define ANALOG_VOLTAGE_MIN -10.0

//! Conversion ratio for Signal-to-Voltage, measured using a short
const double SIGNAL_TO_VOLTAGE_COEFFICIENT = ((ANALOG_VOLTAGE_MAX - ANALOG_VOLTAGE_MIN) / 65535.0);


/*!	\brief Constructs a new GenericInput with the input name and InputPort.
*	\details When using this constructor a default value of 4 readings per ms is used.
*	\note Multiple SignalChannels can share  a single InputPort.  The InputPort represents
*	a DAQ device, for example, whereas the SignalChannel represents one logical signal such
*	as "Position."
*/
GenericInput::GenericInput(QString name, QSharedPointer<InputPort> port ) 
	: SignalChannel(name, 4, port)
{
	for (int i = 0; i < 8; i++)
	{
		addSubchannel(QString("input%1").arg(i), i);
	}
}

/*! \brief Constructs a new GenericInput using all channels not included in the qvUsedChannels vector.
*	\details When using this constructor a default value of 4 readings per ms is used.
*	\note Multiple SignalChannels can share  a single InputPort.  The InputPort represents
*	a DAQ device, for example, whereas the SignalChannel represents one logical signal such
*	as "Position."
*/
GenericInput::GenericInput(QVector<int> &qvUsedChannels, QString name, QSharedPointer<InputPort> port )
	: SignalChannel(name, 4, port)
{
	for (int i = 0; i < 8; i++)
	{
		if (!qvUsedChannels.contains(i))
		{
			addSubchannel(QString("input%1").arg(i), i);
		}
	}
}

QSharedPointer<InputDataUnitPackage> GenericInput::getDataPackage()
{
	QSharedPointer<InputDataUnitPackage> returnVal(new InputDataUnitPackage());
	returnVal->setChannel(getName());
	returnVal->setResolution(msPerSample_);
	returnVal->addData(getValues(), latestUpdateEventOffset());
	if (returnVal->length())
		return returnVal;
	return QSharedPointer<InputDataUnitPackage>();
}

/*!	\brief Returns the values in the RawDataBuffer scaled by the SIGNAL_TO_VOLTAGE_COEFFICIENT.
 */
QMap<QString, QVector<double> > GenericInput::getValues()
{
	QMap<QString, QVector<double> > dataBuffer = getRawValues();

	QMap<QString, QVector<double> >::iterator x = dataBuffer.begin();

	while (x != dataBuffer.end())
	{
		for (QVector<double>::iterator subChanValIter = x.value().begin();
			subChanValIter != x.value().end();
			subChanValIter++)
		{
			(*subChanValIter) *= SIGNAL_TO_VOLTAGE_COEFFICIENT;
		}
		x++;
	}

	return dataBuffer;
}

/*!	\brief Returns the most recent value for the specified subchannel in the RawDataBuffer
 *	scaled by the SIGNAL_TO_VOLTAGE_COEFFICIENT.
 */
double GenericInput::peekValue(QString subchannel)
{
	return SignalChannel::peekValue(subchannel) * SIGNAL_TO_VOLTAGE_COEFFICIENT;
}
