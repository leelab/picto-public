#ifndef _ENGEVENTCOMMANDHANDLER_H_
#define _ENGEVENTCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

#include "FrontPanelInfo.h"

/*! \addtogroup pictoserver_protocol
 * @{
 */

/*! \brief Handles GET commands received specifying use of the HTTP protocol
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