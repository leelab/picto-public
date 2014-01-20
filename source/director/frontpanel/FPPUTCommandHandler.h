#ifndef _FPPUTCOMMANDHANDLER_H_
#define _FPPUTCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"


/*! \brief A Picto::ProtocolCommandHandler that Handles FPPUT commands received by the Director from the EmbeddedFrontPanel.
 *	\details Parses the Picto::ProtocolCommand for the requested value changes (DirectorName, RewardDuration, FlushDuration).
 *	then implements them using various signals.  The object that owns this one should take care of connecting to these signals
 *	and performing the requested operations.
 *
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
struct FPPUTCommandHandler : Picto::ProtocolCommandHandler
{
	Q_OBJECT
public:
	FPPUTCommandHandler();

	QString method() { return QString("FPPUT"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

signals:
	void setName(QString name);
	void setRewardDuration(int controller,int duration);
	void setFlushDuration(int controller,int duration);
};

#endif