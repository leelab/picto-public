#include "BooleanParameter.h"
#include "../memleakdetect.h"

namespace Picto {

/*!	\brief Constructs a Boolean Parameter.
 *	\details A boolean Value Property is created.  TrueLabel and FalseLabel are also created, but not currently used so they are
 *	set as invisible in postDeserialize().
 */
BooleanParameter::BooleanParameter()
: Parameter()
{
	AddDefinableProperty(QVariant::Bool,"Value",QVariant());
	AddDefinableProperty("TrueLabel","True");
	AddDefinableProperty("FalseLabel","False");
}

/*! \brief The NewParameter is not used anymore by anything except the obsolete EngineTest.  It should be removed.
 *	Create() is now the function to use.
 */
Parameter* BooleanParameter::NewParameter()
{
	return new BooleanParameter;
}

/*! \brief Creates a new BooleanParameter and returns a shared Asset pointer to it.
*/
QSharedPointer<Asset> BooleanParameter::Create()
{
	return QSharedPointer<Asset>(new BooleanParameter());
}


void BooleanParameter::postDeserialize()
{
	Parameter::postDeserialize();
	propertyContainer_->getProperty("TrueLabel")->setVisible(false);
	propertyContainer_->getProperty("FalseLabel")->setVisible(false);
	setPropertyRuntimeEditable("Value");
}

bool BooleanParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
