#ifndef _DIRECTORCLICKRESPONSEHANDLER_H_
#define _DIRECTORCLICKRESPONSEHANDLER_H_

#include "../../common/protocol/ClickResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief Implements the ClickResponseHandler class by extracting click data
 *	when it arrives and delivering it to the OperatorClickParameter class.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct  DirectorClickResponseHandler : public Picto::ClickResponseHandler
{
public:
	DirectorClickResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);

};

#endif
