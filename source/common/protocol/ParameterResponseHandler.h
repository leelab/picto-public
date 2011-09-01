#ifndef _PARAMETERRESPONSEHANDLER_H_
#define _PARAMETERRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A response handler for REWARD responses
 */
#if defined WIN32 || defined WINCE
	struct PICTOLIB_API ParameterResponseHandler : public ProtocolResponseHandler
#else
struct ParameterResponseHandler
#endif
{
public:
	ParameterResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

	QString method(){return "PARAMETER";};
protected:
	virtual bool processResponse(QString directive);

};

}; //namespace Picto

#endif
