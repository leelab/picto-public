#include "DirectorErrorResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorErrorResponseHandler::DirectorErrorResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
ErrorResponseHandler(statusManager)
{}

/*! \brief Extends ErrorResponseHandler::processResponse() to display the latest received error message
 *	to the display.
 *	\sa DirectorStatusManager::setUserInfo().
 */
bool DirectorErrorResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setUserInfo("ERROR: "+directive);
	return true;
}