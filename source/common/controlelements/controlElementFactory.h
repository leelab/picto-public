#ifndef _CONTROL_ELEMENT_FACTORY_H_
#define _CONTROL_ELEMENT_FACTORY_H_

#include <QMap>

#include "ControlElement.h"


namespace Picto {

/*! \brief No Longer Used. Factory class for generating specific ControlElement objects
 *
 *	\note Joey: This class is no longer used in a meaningful way in Picto.  It should be removed
 *	by someone who has time for this kind of thing.  In the meantime, it isn't doing any
 *	harm. I have maintained the old documentation below for reference...
 *
 *	\details This factory class is used to generate ControlElements of specific
 *	type.  Before a ControlElement can be generated, it first needs to be 
 *	registered.  Registering a ControlElement requires a name for the 
 *	element and a pointer to the element's NewControlElement function.
 *
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */


#if defined WIN32 || defined WINCE
	class PICTOLIB_API ControlElementFactory
#else
class ControlElementFactory
#endif
{
public:
   typedef ControlElement* (*NewControlElementFnPtr)();

	ControlElementFactory();

	QSharedPointer<ControlElement> generateControlElement(QString type);
	void addControlElementType(QString type, NewControlElementFnPtr newFnPtr);

private:
	static QMap<QString, NewControlElementFnPtr> controlElements_;
};

} //namespace Picto

#endif