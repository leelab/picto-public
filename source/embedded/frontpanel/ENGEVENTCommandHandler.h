#ifndef _ENGEVENTCOMMANDHANDLER_H_
#define _ENGEVENTCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

#include "FrontPanelInfo.h"

/*! \brief Handles ENGEVENT commands received.
 *
 *	ENGEVENT commands are sent from the PictoEngine to the front panel application 
 *	and generally result in some sort of text being desplayed on the front panel
 *	(e.g. "Trial start")
 */
struct ENGEVENTCommandHandler : Picto::ProtocolCommandHandler
{
public:
	ENGEVENTCommandHandler();

	QString method() { return QString("ENGEVENT"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);

	void setPanelInfo(FrontPanelInfo *pInfo) { panelInfo = pInfo; };

private:
	FrontPanelInfo *panelInfo;
};

/*! @} */

#endif