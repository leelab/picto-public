#ifndef _PROTOCOLRESPONSEHANDLER_H_
#define _PROTOCOLRESPONSEHANDLER_H_

#include <QString>
#include <QSharedPointer>
#include "../common.h"
#include "ProtocolResponse.h"
#include "../Network/ComponentStatusManager.h"

namespace Picto {

/*! \brief A base class from which all response handlers are derived
 *
 *	A response handler needs to implement two functions.  The method function is 
 *	simply the name of the directive (e.g. NEWSESSION).  The processCommand function is where
 *	all of the logic is that needs to be triggered when the input directive is received.  This
 *	base classes acceptResponse() function is called by the outside world.  It updates the status
 *	in the ComponentStatusManager, then calls processResponse().
 */
#if defined WIN32 || defined WINCE
struct PICTOLIB_API ProtocolResponseHandler
#else
struct ProtocolResponseHandler
#endif
{
public:
	ProtocolResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);

	virtual QString method() = 0;
	void acceptResponse(QSharedPointer<ProtocolResponse> response);

protected:
	virtual bool processResponse(QString directive) = 0;
	QSharedPointer<ComponentStatusManager> statusManager_;
	bool processingResponse_;
	
};

}; //namespace Picto

#endif
