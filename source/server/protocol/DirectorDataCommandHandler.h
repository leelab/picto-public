/*! \brief Handles the DIRECTORDATA commands, which are sent by Director
 *
 *	PictoDirector sends these commands to the server when it has data it would
 *	like to be recorded.  Initiall, these commands will contain behavioral data,
 *	but it's likely that there will be additional data being transmitted in the 
 *	future.
 *
 *	The id included in the response is to allow the matching of commands and responses.
 *
 *	FORMAT
 *		DIRECTORDATA directorname PICTO.1/0
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
 *		404:NotFound.  In both cases, the content of the response includes an
 *		XML fragment that looks like this:
 *			<DirectorData id=132/>
 *		This is so that responses can be matched with commands, and missed data
 *		can be resent.
 */

#ifndef _DIRECTORDATA_COMMAND_HANDLER_H_
#define _DIRECTORUPDATE__DIRECTORDATA_COMMAND_HANDLER_H_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

struct DirectorDataCommandHandler : Picto::ProtocolCommandHandler
{
public:
	DirectorDataCommandHandler();

	QString method() { return QString("DIRECTORDATA"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
