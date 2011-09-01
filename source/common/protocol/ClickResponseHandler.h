#ifndef _CLICKRESPONSEHANDLER_H_
#define _CLICKRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A response handler for REWARD responses
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API ClickResponseHandler : public ProtocolResponseHandler
#else
struct ClickResponseHandler
#endif
{
public:
	ClickResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

	QString method(){return "CLICK";};
protected:
	virtual bool processResponse(QString directive);

};

}; //namespace Picto

#endif
