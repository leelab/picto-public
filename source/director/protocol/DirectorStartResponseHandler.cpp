#include "DirectorStartResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorStartResponseHandler::DirectorStartResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
StartResponseHandler(statusManager)
{}

bool DirectorStartResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	QSharedPointer<Picto::Experiment> experiment = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getExperiment();
	if(!engine.isNull() && !experiment.isNull())
	{
		statusManager_.toStrongRef()->setStatus(running);
		experiment->runTask(directive);
	}
	statusManager_.toStrongRef()->setStatus(stopped);
	statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setUserInfo("Engine Stopped");
	return false;

}