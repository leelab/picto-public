#ifndef _DIRECTORPARAMETERRESPONSEHANDLER_H_
#define _DIRECTORPARAMETERRESPONSEHANDLER_H_

#include "../../common/protocol/ParameterResponseHandler.h"
#include "../network/DirectorStatusManager.h"

/*! \brief Implements ParameterResponseHandler to set initValues received over the network 
 *	into the correct Experimental Properties.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
