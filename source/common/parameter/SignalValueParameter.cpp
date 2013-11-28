#include "SignalValueParameter.h"
#include "../memleakdetect.h"

namespace Picto {

QHash<QString,QHash<QString,double>> SignalValueParameter::signalData_;
/*! \brief Constructs a new SignalValueParameter
 *	\details Adds a Signal Property for defining which signal value will be
 *	tracked by this SignalValueParameter.  The default signal is the "Position"
 *	signal.
 */
SignalValueParameter::SignalValueParameter()
: Parameter()
{
	AddDefinableProperty(QVariant::String,"Signal","Position");
}

/*! \brief Creates a new SignalValueParameter and returns a shared Asset pointer to it.
*/
QSharedPointer<Asset> SignalValueParameter::Create()
{
	QSharedPointer<Asset> newParam(new SignalValueParameter());
	return newParam;
}

/*! \brief Used (by the Engine::PictoEngine) to add the latest signal values to this SignalValueParameter class.
 *	\details signal is the name of the signal to whom the value belongs.  subChannel is the name of the
 *	sub-channel whose value is being provided.  value is the floating point value.
 *	\note The fact that this is implemented in a static data structure is not very clean architecturally speaking,
 *	We should think about changing it.  See the SignalValueParameter main documentation for more detail.
 */
void SignalValueParameter::setLatestValue(QString signal, QString subChannel, double value)
{
	signalData_[signal.toLower()][subChannel.toLower()] = value;
}

/*! \brief Returns the latest value for the input subChannel name of the signal handled by this Parameter
*/
double SignalValueParameter::getValue(QString subChannel)
{
	QString sigName = propertyContainer_->getPropertyValue("Signal").toString().toLower();
	QString subChanLower = subChannel.toLower();
	if(!signalData_.contains(sigName))
		return 0.0;
	QHash<QString,double>* signalHash = &signalData_[sigName];
	if(!(*signalHash).contains(subChanLower))
	{
		if((*signalHash).size() == 1)
		{
			return (*signalHash).begin().value();
		}
		return 0.0;
	}
	return (*signalHash)[subChanLower];
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
