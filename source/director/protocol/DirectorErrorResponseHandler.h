#ifndef _DIRECTORERRORRESPONSEHANDLER_H_
#define _DIRECTORERRORRESPONSEHANDLER_H_

#include "../../common/protocol/ErrorResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief Extends ErrorResponseHandler to display received error messages to the display.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct DirectorErrorResponseHandler : public Picto::ErrorResponseHandler
{
public:
	DirectorErrorResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};

#endif
