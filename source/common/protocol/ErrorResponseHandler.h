#ifndef _ERRORRESPONSEHANDLER_H_
#define _ERRORRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A response handler for ERROR responses
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
