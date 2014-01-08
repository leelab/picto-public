#ifndef _DIRECTORNEWSESSIONRESPONSEHANDLER_H_
#define _DIRECTORNEWSESSIONRESPONSEHANDLER_H_

#include "../../common/protocol/NewSessionResponseHandler.h"
#include "../../common/network/commandchannel.h"
#include "../network/DirectorStatusManager.h"

/*! \brief Extends NewSessionResponseHandler to setup the Picto::Engine::PictoEngine for the new Session.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct DirectorNewSessionResponseHandler : public Picto::NewSessionResponseHandler
{
public:
	DirectorNewSessionResponseHandler(QSharedPointer<DirectorStatusManager> statusManager, QSharedPointer<Picto::CommandChannel> commandChannel);

protected:
	virtual bool processResponse(QString directive);

};

#endif
