#include "DirectorStopResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorStopResponseHandler::DirectorStopResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
StopResponseHandler(statusManager)
{}

bool DirectorStopResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	if(engine.isNull())
		return true;
	engine->stop();
	statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setUserInfo("Engine Stopped");
	return false;	// status will be set to stopped by START handler when it exits and engine actually stops
}