#ifndef _OKRESPONSEHANDLER_H_
#define _OKRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A response handler for OK responses
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API OKResponseHandler : public ProtocolResponseHandler
#else
struct OKResponseHandler
#endif
{
public:
	OKResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

	QString method(){return "OK";};
protected:
	virtual bool processResponse(QString directive);

};

}; //namespace Picto

#endif
