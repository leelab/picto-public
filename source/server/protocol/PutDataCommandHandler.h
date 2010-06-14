/*! \brief Handles the PUTDATA commands, which are sent by Director
 *
 *	PictoDirector sends these commands to the server when it has data it would
 *	like to be recorded.  Initially, these commands will contain behavioral data,
 *	but it's likely that there will be additional data being transmitted in the 
 *	future.
 *
 *	The id included in the command is a consequence of the fact that these
 *	commands are usually sent as "registered" commands, but it is not an 
 *	essential field for the command
 *
 *	FORMAT
 *		PUTDATA directorname PICTO.1/0
 *		Session-ID:{44dcb670-4bea-11df-9879-0800200c9a66}
 *
 *		<!--- This could be a different data element --->
 *		<BehavioralDataStore id=132>
 *			<Data timestamp=123.4324 x=450 y=394/>
 *			<Data timestamp=123.4334 x=457 y=386/>
 *			...
 *		</BehavioralDataStore>
 *
 *	RESPONSES
 *		The response is of type 200 OK if the data was processed and stored
 *		correctly.  If the session ID isn't recognized, the response is
 *		404:NotFound.  
 */

#ifndef _PUTDATA_COMMAND_HANDLER_H_
#define _PUTDATA_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

struct PutDataCommandHandler : Picto::ProtocolCommandHandler
{
public:
	PutDataCommandHandler();

	QString method() { return QString("PUTDATA"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
