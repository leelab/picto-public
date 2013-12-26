#ifndef _PAUSERESPONSEHANDLER_H_
#define _PAUSERESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A base class for ProtocolResponseHandler classes that respond to the "PAUSE" method.
 *	\details PAUSE responses indicate that the Experiment should run to the next PausePoint and
 *	then wait for a RESUME command to come in.
 *
 *	The processResponse() function implemented by this class is empty.  Extend this class
 *	to add logic that is triggered by the arrival of a new ProtocolRespose.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API PauseResponseHandler : public ProtocolResponseHandler
#else
struct PauseResponseHandler
#endif
{
public:
	PauseResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

	QString method(){return "PAUSE";};
protected:
	virtual bool processResponse(QString directive);

};

}; //namespace Picto

#endif
