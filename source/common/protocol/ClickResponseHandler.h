#ifndef _CLICKRESPONSEHANDLER_H_
#define _CLICKRESPONSEHANDLER_H_

#include "ProtocolResponseHandler.h"

namespace Picto {

/*! \brief A base class for ProtocolResponseHandler classes that respond to the "CLICK" method.
 *	\details CLICK responses indicate that the operator clicked inside their remote viewer window.  They
 *	are sent with the location in the window where the click occure.
 *	
 *	The processResponse() function implemented by this class is empty.  Extend this class
 *	to add logic that is triggered by the arrival of a new ProtocolRespose.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
