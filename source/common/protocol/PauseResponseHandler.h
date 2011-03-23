#ifndef _PAUSERESPONSEHANDLER_H_
#define _PAUSERESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A response handler for PAUSE responses
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
