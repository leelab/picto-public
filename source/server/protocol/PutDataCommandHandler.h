#ifndef _PUTDATA_COMMAND_HANDLER_H_
#define _PUTDATA_COMMAND_HANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"
#include <QTime>
#include <QFuture>
#include <QReadWriteLock>

/*! \brief Handles PUTDATA commands sent by Director and Proxy.
 *
 *	\details The Director and Proxy send these commands to the server with data that should be
 *	recorded.  PUTDATA commands are very heavily used and include just about all of the important data 
 *	in a Session: Property value changes, input signal values, frame times, transition traversals, spike data,
 *	LFP data, reward times, etc.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct PutDataCommandHandler : Picto::ProtocolCommandHandler
{
public:
	PutDataCommandHandler();

	QString method() { return QString("PUTDATA"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
//private:
	//QReadWriteLock readWriteLock_;
	//QHash<QString,QFuture<bool>> flushCacheFutures_;
	//QHash<QString,bool> flushCacheWaiting_;
};


#endif
