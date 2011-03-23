#include "DirectorResumeResponseHandler.h"
using namespace Picto;

DirectorResumeResponseHandler::DirectorResumeResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
ResumeResponseHandler(statusManager)
{}

bool DirectorResumeResponseHandler::processResponse(QString directive)
{
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.staticCast<DirectorStatusManager>()->getEngine();
	if(engine.isNull())
		return false;
	engine->resume();
	return true;
}