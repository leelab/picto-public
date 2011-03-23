#ifndef _LOADEXPRESPONSEHANDLER_H_
#define _LOADEXPRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A response handler for LOADEXP responses
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API LoadExpResponseHandler : public ProtocolResponseHandler
#else
struct LoadExpResponseHandler
#endif
{
public:
	LoadExpResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

	QString method(){return "LOADEXP";};
protected:
	virtual bool processResponse(QString directive);

};

}; //namespace Picto

#endif
