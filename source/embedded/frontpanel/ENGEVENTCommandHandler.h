#ifndef _ENGEVENTCOMMANDHANDLER_H_
#define _ENGEVENTCOMMANDHANDLER_H_

#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

#include "FrontPanelInfo.h"

/*! \brief Handles ENGEVENT commands received. NO LONGER USED
 *
 * This object used to allow the director to push data to the FrontPanel application.  It is no longer
 * used. The front panel is now set up as the communications master handling both setting and polling 
 * data.  At a more opportune time, this object should probably be deleted and references to it removed
 * from other parts of the application.
 * \author Mark Hammond, Matt Gay
 * \date 2009-2013
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

#endif