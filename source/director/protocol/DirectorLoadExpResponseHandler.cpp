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
	DesignRoot designRoot;
	if(!designRoot.resetDesignRoot(directive.toUtf8()))
	{
		QString errorInfo;
		if(designRoot.hasError())
		{
			errorInfo = designRoot.getLastError().name + " - " + designRoot.getLastError().details; 
		}
		statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->setUserInfo(QString("Error loading experiment: %1").arg(errorInfo));
		return false;
	}
	//We don't want any Analysis elements in our experiment, remove them.
	for(int i=0;i<designRoot.getNumAnalyses();i++)
	{
		designRoot.removeAnalysis(i);
	}
	designRoot.setUndoPoint();
	designRoot.refreshFromXml();

	designRoot.enableRunMode(true);
	QSharedPointer<Picto::Experiment> experiment = designRoot.getExperiment().staticCast<Experiment>();
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