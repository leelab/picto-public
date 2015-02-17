#include "GenericInput.h"

#include "../memleakdetect.h"

using namespace Picto;

GenericInput::GenericInput(QString name, QSharedPointer<InputPort> port ) 
	: SignalChannel(name, 4, port)
{
	for (int i = 2; i < 8; i++)
	{
		addSubchannel(QString("input%1").arg(i), i);
	}
}

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
