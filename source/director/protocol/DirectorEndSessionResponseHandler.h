#ifndef _DIRECTORENDSESSIONRESPONSEHANDLER_H_
#define _DIRECTORENDSESSIONRESPONSEHANDLER_H_

#include "../../common/protocol/EndSessionResponseHandler.h"
#include "../../common/network/commandchannel.h"
#include "../network/DirectorStatusManager.h"

/*! \brief Extends EndSessionResponseHandler to perform various "finish up" tasks in the 
 *	Picto::Engine::PictoEngine.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct DirectorEndSessionResponseHandler : public Picto::EndSessionResponseHandler
{
public:
	DirectorEndSessionResponseHandler(QSharedPointer<DirectorStatusManager> statusManager, QSharedPointer<Picto::CommandChannel> commandChannel);

protected:
	virtual bool processResponse(QString directive);

};

#endif
