#ifndef _OKRESPONSEHANDLER_H_
#define _OKRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A base class for ProtocolResponseHandler classes that respond to the "OK" method.
 *	\details OK responses are just an all clear message and don't include any information beyong
 *	that.
 *
 *	The processResponse() function implemented by this class is empty.  Extend this class
 *	to add logic that is triggered by the arrival of a new ProtocolRespose.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
