#include "DirectorParameterResponseHandler.h"
#include "../../common/engine/propertyTable.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorParameterResponseHandler::DirectorParameterResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
ParameterResponseHandler(statusManager)
{}

/*! \brief Implements ParameterResponseHandler::processResponse() to set the init value from the input
 *	directive to the correct Property in the current Experiment.
 */
bool DirectorParameterResponseHandler::processResponse(QString directive)
{	int paramIdLoc = directive.indexOf("id=");
	if(paramIdLoc < 0)
		return false;
	paramIdLoc += 3;
	int paramIdEndLoc = directive.indexOf("\n",paramIdLoc);
	if (paramIdEndLoc <= paramIdLoc)
		return false;
	int paramId = directive.mid(paramIdLoc,paramIdEndLoc-paramIdLoc).toInt();
	if(paramId < 0)
		return false;
	QString valStr = directive.mid(paramIdEndLoc+1);
	if(valStr.isEmpty())
		return false;

	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.toStrongRef().staticCast<DirectorStatusManager>()->getEngine();
	QSharedPointer<PropertyTable> propTable = engine->getPropertyTable();
	propTable->updateInitPropertyValue(paramId,valStr);
	//Use ID to set parameter value.
	return true;
}