#ifndef _ERRORRESPONSEHANDLER_H_
#define _ERRORRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A base class for ProtocolResponseHandler classes that respond to the "ERROR" method.
 *	\details Currently ERROR responses are not used, but this is here in case they are used at some point in
 *	the future.
 *
 *	The processResponse() function implemented by this class is empty.  Extend this class
 *	to add logic that is triggered by the arrival of a new ProtocolRespose.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API ErrorResponseHandler : public ProtocolResponseHandler
#else
struct ErrorResponseHandler
#endif
{
public:
	ErrorResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

	QString method(){return "ERROR";};
protected:
	virtual bool processResponse(QString directive);

};

}; //namespace Picto

#endif
