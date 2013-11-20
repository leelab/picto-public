#include <QString>
#include <QByteArray>
#include <QXmlStreamReader>

#include <QTextStream>

#include "ENGEVENTCommandHandler.h"


ENGEVENTCommandHandler::ENGEVENTCommandHandler()
{
}

/*!	The command handler for engine events takes information from the engine, 
 *	and updates the front panel software.  Generally speaking, incoming events
 *	will result in some sort of change on the status display (assuming that the
 *	status display is on).  Given the speed at which events are generated, I am
 *	erring on the side of caution when it comes to processing these events.  For
 *	example, a trial start event does not change the display other than simply
 *	resetting the trial counter (otherwise, the display would be a constant
 *	stream of trialstart/reward/trialend...).
 *	
 *	Most of the panel info functions are self explanatory, but the "lastEvent"
 *	deserves some explanation. LastEvent is the string that will be displayed on 
 *	the second line of the status display.  For the moment, that string will
 *	rarely be changed (again trying to keep the display clean).  With real
 *	experiments, it is likely that the users will use it to display status info.
 */
QSharedPointer<Picto::ProtocolResponse> ENGEVENTCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	//return a null pointer (since ENGEVENTs are never responded to)
	return QSharedPointer<Picto::ProtocolResponse>();
}
