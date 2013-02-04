#include "DirectorLoadExpResponseHandler.h"
#include "../../common/design/designroot.h"
#include "../../common/memleakdetect.h"

using namespace Picto;

DirectorLoadExpResponseHandler::DirectorLoadExpResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
LoadExpResponseHandler(statusManager)
{}

bool DirectorLoadExpResponseHandler::processResponse(QString directive)
{
	Q_ASSERT(!statusManager_.isNull());
	//Load the experiment
	DesignRoot design;
	if(!design.resetDesignRoot(directive.toUtf8()))
	{
		QString errorInfo;
		if(design.hasError())
		{
			errorInfo = design.getLastError().name + " - " + design.getLastError().details; 
		}
		statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setUserInfo(QString("Error loading experiment: %1").arg(errorInfo));
		return false;
	}
	QSharedPointer<Picto::Experiment> experiment = design.getDesign("Experiment",0)->getRootAsset().staticCast<Experiment>();
	if(!experiment)
	{
		statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setUserInfo(QString("Error loading experiment: Design did not contain experiment"));
		return false;
	}
	statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setExperiment(experiment);

	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setUserInfo("Loaded experiment, Session ID: " + engine->getSessionId().toString());
	experiment->setEngine(engine);
	return true;
}