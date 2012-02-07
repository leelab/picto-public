#include "DirectorPauseResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorPauseResponseHandler::DirectorPauseResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
PauseResponseHandler(statusManager)
{}

bool DirectorPauseResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	if(engine.isNull())
		return true;
	//engine->schedulePause();
	engine->pause();
	return true;
}