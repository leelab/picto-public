#ifndef _DIRECTORLOADEXPRESPONSEHANDLER_H_
#define _DIRECTORLOADEXPRESPONSEHANDLER_H_

#include "../../common/protocol/LoadExpResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief Implements LoadExpResponseHandler to load the Experiment in directive's received over the network.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct DirectorLoadExpResponseHandler : public Picto::LoadExpResponseHandler
{
public:
	DirectorLoadExpResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);
};

#endif
