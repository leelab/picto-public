#ifndef _STARTRESPONSEHANDLER_H_
#define _STARTRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A base class for ProtocolResponseHandler classes that respond to the "START" method.
 *	\details The START response indicates that a Task Run should be started.  The message includes the name of the
 *	Task to be run,
 *
 *	The processResponse() function implemented by this class is empty.  Extend this class
 *	to add logic that is triggered by the arrival of a new ProtocolRespose.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API StartResponseHandler : public ProtocolResponseHandler
#else
struct StartResponseHandler
#endif
{
public:
	StartResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

	QString method(){return "START";};
protected:
	virtual bool processResponse(QString directive);

};

}; //namespace Picto

#endif
