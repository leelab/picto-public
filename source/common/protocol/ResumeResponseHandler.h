#ifndef _RESUMERESPONSEHANDLER_H_
#define _RESUMERESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A response handler for RESUME responses
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API ResumeResponseHandler : public ProtocolResponseHandler
#else
struct ResumeResponseHandler
#endif
{
public:
	ResumeResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

	QString method(){return "RESUME";};
protected:
	virtual bool processResponse(QString directive);

};

}; //namespace Picto

#endif
