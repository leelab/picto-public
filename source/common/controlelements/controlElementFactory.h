#ifndef _CONTROL_ELEMENT_FACTORY_H_
#define _CONTROL_ELEMENT_FACTORY_H_

#include <QMap>

#include "ControlElement.h"


namespace Picto {

/*! \brief Factory class for generating specific ControlElement objects
 *
 *	This factory class is used to generate ControlElements of specific
 *	type.  Before a ControlElement can be generated, it first needs to be 
 *	registered.  Registering a ControlElement requires a name for the 
 *	element and a pointer to the element's NewControlElement function.
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