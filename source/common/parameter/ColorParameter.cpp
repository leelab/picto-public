#include "ColorParameter.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a ColorParameter
 *	\details Adds a Color Property called Value to hold the current color.
 */
ColorParameter::ColorParameter()
: Parameter()
{
	AddDefinableProperty(QVariant::Color,"Value",QVariant());
}

/*! \brief The NewParameter is not used anymore by anything except the obsolete EngineTest.  It should be removed.
 *	Create() is now the function to use.
 */
Parameter* ColorParameter::NewParameter()
{
	return new ColorParameter;
}

/*! \brief Creates a new ColorParameter and returns a shared Asset pointer to it.
*/
QSharedPointer<Asset> ColorParameter::Create()
{
	return QSharedPointer<Asset>(new ColorParameter());
}


void ColorParameter::postDeserialize()
{
	Parameter::postDeserialize();
	setPropertyRuntimeEditable("Value");
}

bool ColorParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
