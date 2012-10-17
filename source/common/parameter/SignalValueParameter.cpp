#include "SignalValueParameter.h"
#include "../memleakdetect.h"

namespace Picto {

QHash<QString,QHash<QString,double>> SignalValueParameter::signalData_;

SignalValueParameter::SignalValueParameter()
: Parameter()
{
	AddDefinableProperty(QVariant::String,"Signal","Position");
}

QSharedPointer<Asset> SignalValueParameter::Create()
{
	QSharedPointer<Asset> newParam(new SignalValueParameter());
	return newParam;
}

//Used to add the latest signal values
void SignalValueParameter::setLatestValue(QString signal, QString subChannel, double value)
{
	signalData_[signal][subChannel] = value;
}

//Returns the latest value on the input subChannel for this parameters signal
double SignalValueParameter::getValue(QString subChannel)
{
	QString sigName = propertyContainer_->getPropertyValue("Signal").toString();
	if(!signalData_.contains(sigName))
		return 0.0;
	QHash<QString,double>* signalHash = &signalData_[sigName];
	if(!(*signalHash).contains(subChannel))
	{
		if((*signalHash).size() == 1)
		{
			return (*signalHash).begin().value();
		}
		return 0.0;
	}
	return (*signalHash)[subChannel];
}

void SignalValueParameter::postDeserialize()
{
	Parameter::postDeserialize();
}

bool SignalValueParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
