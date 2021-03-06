#include "DirectorPauseResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorPauseResponseHandler::DirectorPauseResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
PauseResponseHandler(statusManager)
{}

/*! \brief Implements PauseResponseHandler::processResponse() to pause the current Experiment when a directive
 *	is received.
 */
bool DirectorPauseResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	if(engine.isNull())
		return true;
	engine->pause();
	return true;
}