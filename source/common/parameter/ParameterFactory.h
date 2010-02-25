/*! \brief Factory class for generating specific Parameter objects
 *
 *	This factory class is used to generate Parameters of specific
 *	type.  Before a Parameter can be generated, it first needs to be 
 *	registered.  Registering a Parameter requires a name for the 
 *	element and a pointer to the element's NewParameter function.
 */

#ifndef _PARAMETER_FACTORY_H_
#define _PARAMETER_FACTORY_H_

#include <QMap>

#include "Parameter.h"


namespace Picto {

#if defined WIN32 || defined WINCE
	class PICTOLIB_API ParameterFactory
#else
class ParameterFactory
#endif
{
public:
   typedef Parameter* (*NewParameterFnPtr)();

	ParameterFactory();

	QSharedPointer<Parameter> generateParameter(QString type);
	void addParameterType(QString type, NewParameterFnPtr newFnPtr);

private:
	static QMap<QString, NewParameterFnPtr> parameters_;
};

} //namespace Picto

#endif