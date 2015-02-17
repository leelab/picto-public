#include "Parameter.h"
#include "../memleakdetect.h"

namespace Picto {

Parameter::Parameter() :
	fixingValues_(false)
{
}

/*! \brief Returns true if this Parameters current Property values are valid.  Returns false otherwise.
 *	\details If this function returns false, the design will not pass validation (ie. compilation).  Also,
 *	if the function returns false, a string describing the problem is loaded into the warning reference for
 *	printing out to the validation error window.
 */
bool Parameter::valuesAreValid(QString& warning)
{
	UNREFERENCED_PARAMETER(warning);
	return true;
}
/*! \brief Fixes values that are outside of valid ranges.
 *	\details This is used during a live experiment to make sure that runtime editable values stay within their valid ranges.
 *	\note There is some kind of bug here.  We have noticed that even when values are updated by this function, the fixed values
 *	are not necessarily reflected either in the PropertyFrame or PropertyBrowser.  This is worth looking into in the future.
 */
void Parameter::fixValues()
{
}


void Parameter::postDeserialize()
{
	Scriptable::postDeserialize();
}

/*! \brief Extends Scriptable::validateObject() to verify that the Property values of this Parameter are within
 *	acceptable ranges.
 *	\details valuesAreValid() is used for this.
 */
bool Parameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Scriptable::validateObject(xmlStreamReader))
		return false;
	QString warning;
	if(!valuesAreValid(warning))
	{
		if(warning.isEmpty())
			warning = "Invalid property value detected.";
		addError(warning);
		return false;
	}
	return true;
}

}; //namespace Picto
