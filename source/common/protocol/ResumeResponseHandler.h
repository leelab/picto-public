#ifndef _RESUMERESPONSEHANDLER_H_
#define _RESUMERESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A base class for ProtocolResponseHandler classes that respond to the "RESUME" method.
 *	\details A RESUME response indicates that a paused Experiment should resume operation.
 *
 *	The processResponse() function implemented by this class is empty.  Extend this class
 *	to add logic that is triggered by the arrival of a new ProtocolRespose.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
