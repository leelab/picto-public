#ifndef _STARTRESPONSEHANDLER_H_
#define _STARTRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A response handler for START responses
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
