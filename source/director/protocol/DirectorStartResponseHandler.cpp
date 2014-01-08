#include "DirectorStartResponseHandler.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorStartResponseHandler::DirectorStartResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
StartResponseHandler(statusManager)
{}

/*! \brief Implements StartResponseHandler::processResponse() to run the Task whose name is included in
 *	the directive.
 *	\note This function blocks.  The entire Task will run from inside this function and this function
 *	will only return after the Task ends.  This is not a clean run model, and we should 
 *	fix it at some point such that this function is merely a part of the Qt event loop and serves only
 *	to schedule that the task be run.  See Picto::CommandChannel::processResponses() for more details.
 */
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