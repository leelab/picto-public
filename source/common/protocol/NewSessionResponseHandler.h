#ifndef _NEWSESSIONRESPONSEHANDLER_H_
#define _NEWSESSIONRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"
#include "../network/commandchannel.h"

namespace Picto {

/*! \brief A response handler for NEWSESSION responses.
 *	\details NEWSESSION responses are requests to a Picto Component to perform whatever initializations are necessary 
 *	to start a new session.  The message comes in with the SessionId that will be used for the new session.  This class handles that job.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API NewSessionResponseHandler : public ProtocolResponseHandler
#else
struct NewSessionResponseHandler
#endif
{
public:
	NewSessionResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<CommandChannel> commandChannel);

	QString method(){return "NEWSESSION";};
protected:
	virtual bool processResponse(QString directive);
	QWeakPointer<CommandChannel> commandChannel_;
};

}; //namespace Picto

#endif
