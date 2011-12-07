#ifndef _ENDSESSIONRESPONSEHANDLER_H_
#define _ENDSESSIONRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"
#include "../network/commandchannel.h"

namespace Picto {

/*! \brief A response handler for ENDSESSION responses
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API EndSessionResponseHandler : public ProtocolResponseHandler
#else
struct EndSessionResponseHandler
#endif
{
public:
	EndSessionResponseHandler(QSharedPointer<ComponentStatusManager> statusManager, QSharedPointer<CommandChannel> commandChannel);

	QString method(){return "ENDSESSION";};
protected:
	virtual bool processResponse(QString directive);
	QWeakPointer<CommandChannel> commandChannel_;	//Command channel owns this, not the other way around
private:

};

}; //namespace Picto

#endif
