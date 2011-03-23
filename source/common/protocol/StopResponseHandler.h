#ifndef _STOPRESPONSEHANDLER_H_
#define _STOPRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A response handler for STOP responses
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
