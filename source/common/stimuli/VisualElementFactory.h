/*! \brief Factory class for generating specific visual elements
 *
 *	This factory class is used to generate Visual Elements of specific
 *	type.  Before a VisualElement can be generated, it first needs to be 
 *	registered.  Registering a VisualElement requires a name for the 
 *	element and a pointer to the element's NewVisualElement function.
 */

#ifndef _VISUAL_ELEMENT_FACTORY_H_
#define _VISUAL_ELEMENT_FACTORY_H_

#include <QMap>

#include "VisualElement.h"


namespace Picto {

#if defined WIN32 || defined WINCE
	class PICTOLIB_API VisualElementFactory
#else
class VisualElementFactory
#endif
{
public:
   typedef VisualElement* (*NewVisualElementFnPtr)();

	VisualElementFactory();

	QSharedPointer<VisualElement> generateVisualElement(QString type);
	void addVisualElementType(QString type, NewVisualElementFnPtr newFnPtr);

private:
	static QMap<QString, NewVisualElementFnPtr> visualElements_;
};

} //namespace Picto

#endif