#ifndef _DIRECTORPARAMETERRESPONSEHANDLER_H_
#define _DIRECTORPARAMETERRESPONSEHANDLER_H_

#include "../../common/protocol/ParameterResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief A response handler for REWARD responses
 */
struct  DirectorParameterResponseHandler : public Picto::ParameterResponseHandler
{
public:
	DirectorParameterResponseHandler(QSharedPointer<DirectorStatusManager> statusManager);

	QString method(){return "PARAMETER";};
protected:
	virtual bool processResponse(QString directive);

};

#endif
