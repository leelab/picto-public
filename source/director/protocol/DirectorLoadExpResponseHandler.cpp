#include "DirectorLoadExpResponseHandler.h"
#include "../../common/memleakdetect.h"

using namespace Picto;

DirectorLoadExpResponseHandler::DirectorLoadExpResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
LoadExpResponseHandler(statusManager)
{}

bool DirectorLoadExpResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	//Load the experiment
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(directive.toUtf8()));
	QSharedPointer<Picto::Experiment> experiment(Picto::Experiment::Create());

	xmlReader->readNext();
	while(!xmlReader->atEnd() && xmlReader->name().toString() != "Experiment")
	{
		xmlReader->readNext();
	}

	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	QList<QSharedPointer<Picto::RenderingTarget> > renderingTargets = engine->getRenderingTargets();
	if(!experiment->fromXml(xmlReader))
	{
		statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setUserInfo(QString("Error loading experiment: %1").arg(experiment->getErrors()));
		return false;
	}
	else
	{
		statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setExperiment(experiment);
		statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setUserInfo("Loaded experiment, Session ID: " + engine->getSessionId().toString());
		QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
		experiment->setEngine(engine);
	}
	return true;
}