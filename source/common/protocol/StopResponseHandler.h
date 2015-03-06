#ifndef _STOPRESPONSEHANDLER_H_
#define _STOPRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A base class for ProtocolResponseHandler classes that respond to the "STOP" method.
 *	\details A STOP response indicates that the currently running Task should be stopped.
 *
 *	The processResponse() function implemented by this class is empty.  Extend this class
 *	to add logic that is triggered by the arrival of a new ProtocolRespose.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API StopResponseHandler : public ProtocolResponseHandler
#else
struct StopResponseHandler
#endif
{
public:
	StopResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

	QString method(){return "STOP";};
protected:
	virtual bool processResponse(QString directive);

};

}; //namespace Picto

#endif
