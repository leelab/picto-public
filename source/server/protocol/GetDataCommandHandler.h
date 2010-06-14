/*! \brief Handles the GETDATA commands, which are sent by Director
 *
 *	PictoWorkstation sends these commands to the server to request that 
 *  behavioral data be sent.  The target of the command is the timestamp
 *	from which data should be returned (e.g. a target of 10.324 will 
 *  return all data that was collected after 10.324 seconds).
 *
 *	FORMAT
 *		GETDATA time(e.g. 3.245) PICTO.1/0
 *		Session-ID:{44dcb670-4bea-11df-9879-0800200c9a66}
 *
 *	RESPONSES
 *
 *	The response to this command is always of type 200:OK, and the content
 *	will be an XML fragment containing behavioral and state change data.
 *	It is possible that in future versions, the response may contain additional 
 *	data types.
 *
 *	<Data>
 *	  <BehavioralDataStore id=143>
 *	    <Data timestamp=123.4324 x=450 y=394/>
 *	    <Data timestamp=123.4334 x=457 y=386/>
 *	    ...
 *	  </BehavioralDataStore>
 *	  <StateChangeDataStore>
 *	    <!--- This hasn't yet been coded, so the format may change --->
 *	  </StateChangeDataStore>
 *	</Data>
 */

#ifndef _GETDATA_COMMAND_HANDLER_H_
#define _GETDATA_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

struct GetDataCommandHandler : Picto::ProtocolCommandHandler
{
public:
	GetDataCommandHandler();

	QString method() { return QString("GETDATA"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
};


#endif
