#include "DirectorLoadExpResponseHandler.h"

using namespace Picto;

DirectorLoadExpResponseHandler::DirectorLoadExpResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
LoadExpResponseHandler(statusManager)
{}

bool DirectorLoadExpResponseHandler::processResponse(QString directive)
{
	//Load the experiment
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(directive.toUtf8()));
	QSharedPointer<Picto::Experiment> experiment(new Picto::Experiment);

	xmlReader->readNext();
	while(!xmlReader->atEnd() && xmlReader->name().toString() != "Experiment")
	{
		xmlReader->readNext();
	}

	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.staticCast<DirectorStatusManager>()->getEngine();
	QList<QSharedPointer<Picto::RenderingTarget> > renderingTargets = engine->getRenderingTargets();
	if(!experiment->fromXml(xmlReader))
	{
		statusManager_.staticCast<DirectorStatusManager>()->updateSplashStatus(QString("Error loading experiment: %1").arg(experiment->getErrors()));
		return false;
	}
	else
	{
		statusManager_.staticCast<DirectorStatusManager>()->setExperiment(experiment);
		statusManager_.staticCast<DirectorStatusManager>()->updateSplashStatus("Loaded experiment, Session ID: " + engine->getSessionId().toString());
	}
	return true;
}