#ifndef _DIRECTORSTOPRESPONSEHANDLER_H_
#define _DIRECTORSTOPRESPONSEHANDLER_H_

#include "../../common/protocol/StopResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief Implements StopResponseHandler to stop the current Task run when a stop request is
 *	received over the network.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct DirectorStopResponseHandler : public Picto::StopResponseHandler
{
public:
	DirectorStopResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);
};

#endif
