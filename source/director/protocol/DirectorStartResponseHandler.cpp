#include "DirectorStartResponseHandler.h"
using namespace Picto;

DirectorStartResponseHandler::DirectorStartResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
StartResponseHandler(statusManager)
{}

bool DirectorStartResponseHandler::processResponse(QString directive)
{
	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.staticCast<DirectorStatusManager>()->getEngine();
	QSharedPointer<Picto::Experiment> experiment = statusManager_.staticCast<DirectorStatusManager>()->getExperiment();
	if(!engine.isNull() && !experiment.isNull())
	{
		statusManager_->setStatus(running);
		experiment->runTask(directive, engine);
	}
	statusManager_->setStatus(stopped);
	statusManager_.staticCast<DirectorStatusManager>()->updateSplashStatus("Engine Stopped");
	return false;

}