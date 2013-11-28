#ifndef _PARAMETER_FACTORY_H_
#define _PARAMETER_FACTORY_H_

#include <QMap>

#include "Parameter.h"


namespace Picto {

/*! \brief This class is no longer used and should probably be removed.  Picto has changed a lot since it was created.  
 *	The original documentation appears below for reference.
 *
 *	This factory class is used to generate Parameters of specific
 *	type.  Before a Parameter can be generated, it first needs to be 
 *	registered.  Registering a Parameter requires a name for the 
 *	element and a pointer to the element's NewParameter function.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
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