/*! \file AnnounceCommandHandler.h
 * \ingroup picto_protocol
 * \brief
 */

#ifndef _ANNOUNCECOMMANDHANDLER_H_
#define _ANNOUNCECOMMANDHANDLER_H_

#include "../common.h"

#include "ProtocolCommandHandler.h"

namespace Picto {

/*! \addtogroup picto_engine
 * @{
 */

struct PICTOLIB_CLASS AnnounceCommandHandler : ProtocolCommandHandler
{
public:
	AnnounceCommandHandler();

	QString method() { return QString("ANNOUNCE"); }
	QSharedPointer<ProtocolResponse> processCommand(QSharedPointer<ProtocolCommand>);
};

/*! @} */

}; //namespace Picto

#endif
