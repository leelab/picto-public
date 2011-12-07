#ifndef _NEWSESSIONRESPONSEHANDLER_H_
#define _NEWSESSIONRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"
#include "../network/commandchannel.h"

namespace Picto {

/*! \brief A response handler for NEWSESSION responses
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
