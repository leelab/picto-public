#ifndef _PROTOCOLRESPONSEHANDLER_H_
#define _PROTOCOLRESPONSEHANDLER_H_

#include <QString>
#include <QSharedPointer>
#include "../common.h"
#include "ProtocolResponse.h"
#include "../Network/ComponentStatusManager.h"

namespace Picto {

/*! \brief A base class for objects that handle ProtocolResponses coming in over the network in 
 *	response to previously sent ProtocolCommands.
 *
 *	A ProtocolResponseHandler needs to implement two functions.  The method function is 
 *	simply the name of the directive in the ProtocolResponse (e.g. NEWSESSION).  
 *	The processCommand function is where child classes can put all of the logic that needs to be triggered when the 
 *	input directive arrives.  
 *	
 *	This class also defines an acceptResponse() function which is the one called by the outside world
 *	when a ProtocolResponse arrives.  acceptResponse() takes care of updating the ComponentStatusManager's status
 *	as a function of the incoming ProtocolResponse.  The acceptResponse() function is the one that calls 
 *	processResponse().
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
struct PICTOLIB_API ProtocolResponseHandler
#else
struct ProtocolResponseHandler
#endif
{
public:
	ProtocolResponseHandler(QSharedPointer<ComponentStatusManager> statusManager);
	
	/*! \brief Returns the name of the ProtocolResponse that this ProtocolResponseHandler handles.*/
	virtual QString method() = 0;
	void acceptResponse(QSharedPointer<ProtocolResponse> response);

protected:
	/*! \brief Processes the input directive from the latest ProtocolResponse and triggers any necessary logic based on that directive.
	 *	\details The directive input here is everything in the ProtocolReponseHandler content beyond the original string 
	 *	that matches method().  For example, a Start Task message has content "START taskname".  In this case, taskname would be the 
	 *	input into this function.  
	 *
	 *	This function returns true if the directive was processed correctly and false if the class could not respond to the directive.
	 *	\note If processResponse() returns false, the status change that would normally be triggered in the ComponentStatusManager will
	 *	not be triggered.
	 */
	virtual bool processResponse(QString directive) = 0;
	QWeakPointer<ComponentStatusManager> statusManager_;	//Status manager owns this, not the other way around
	bool processingResponse_;
	
};

}; //namespace Picto

#endif
