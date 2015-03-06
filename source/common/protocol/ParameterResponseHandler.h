#ifndef _PARAMETERRESPONSEHANDLER_H_
#define _PARAMETERRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A base class for ProtocolResponseHandler classes that respond to the "PARAMETER" method.
 *	\details PARAMETER messages indicate that the Operator changed the initValue of one of the
 *	runtime Properties exposed during an experiment.  The response includes the AssetId of the
 *	affected Property and the new value.
 *
 *	The processResponse() function implemented by this class is empty.  Extend this class
 *	to add logic that is triggered by the arrival of a new ProtocolRespose.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API ParameterResponseHandler : public ProtocolResponseHandler
#else
struct ParameterResponseHandler
#endif
{
public:
	ParameterResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

	QString method(){return "PARAMETER";};
protected:
	virtual bool processResponse(QString directive);

};

}; //namespace Picto

#endif
