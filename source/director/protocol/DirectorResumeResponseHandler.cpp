#include "DirectorResumeResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorResumeResponseHandler::DirectorResumeResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
ResumeResponseHandler(statusManager)
{}

/*! \brief Implements ResumeResponseHandler::processResponse() to resume running the current Experiment 
 *	from a paused state.
 */
bool DirectorResumeResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	if(engine.isNull())
		return false;
	engine->play();
	return true;
}