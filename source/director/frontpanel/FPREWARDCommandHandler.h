#ifndef _FPREWARDCOMMANDHANDLER_H_
#define _FPREWARDCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief A Picto::ProtocolCommandHandler that Handles FPREWARD commands received by the Director from the EmbeddedFrontPanel.
 *	\details Emits a giveReward() signal when a reward is requested.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct FPREWARDCommandHandler : Picto::ProtocolCommandHandler
{
	Q_OBJECT
public:
	FPREWARDCommandHandler();

	QString method() { return QString("FPREWARD"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

signals:
	void giveReward(int controller);
};

#endif