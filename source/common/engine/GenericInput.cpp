#include "GenericInput.h"

#include "../memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new GenericInput with the input name and InputPort.
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
