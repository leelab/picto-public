#ifndef _DIRECTORRESUMERESPONSEHANDLER_H_
#define _DIRECTORRESUMERESPONSEHANDLER_H_

#include "../../common/protocol/ResumeResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief Implements ResumeResponseHandler to resume running the current Experiment 
 *	from a paused state when a resume request is received over the network.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct DirectorResumeResponseHandler : public Picto::ResumeResponseHandler
{
public:
	DirectorResumeResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

protected:
	virtual bool processResponse(QString directive);
};

#endif
