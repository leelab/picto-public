#include "NumericParameter.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto
{

/*! \brief Constructs a NumericParameter
 *	\details Adds a Double Property called Value to hold the current value.
 */
NumericParameter::NumericParameter()
: 
  value_(0),
  units_(""),
  Parameter()
{
	AddDefinableProperty(QVariant::Int,"Value",QVariant());
	AddDefinableObjectFactory("Units",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));


}

/*! \brief The NewParameter is not used anymore by anything except the obsolete EngineTest.  It should be removed.
 *	Create() is now the function to use.
 */
Parameter* NumericParameter::NewParameter()
{
	return new NumericParameter;
}

/*! \brief Creates a new NumericParameter and returns a shared Asset pointer to it.
*/
QSharedPointer<Asset> NumericParameter::Create()
{
	return QSharedPointer<Asset>(new NumericParameter());
}

void NumericParameter::postDeserialize()
{
	Parameter::postDeserialize();
	setPropertyRuntimeEditable("Value");
}

bool NumericParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

} //namespace Picto