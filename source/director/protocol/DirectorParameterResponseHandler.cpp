#include "DirectorParameterResponseHandler.h"
#include "../../common/engine/propertyTable.h"
using namespace Picto;

DirectorParameterResponseHandler::DirectorParameterResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
ParameterResponseHandler(statusManager)
{}

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

	QSharedPointer<Picto::Engine::PictoEngine> engine = statusManager_.staticCast<DirectorStatusManager>()->getEngine();
	QSharedPointer<PropertyTable> propTable = engine->getPropertyTable();
	propTable->updatePropertyValue(paramId,valStr);
	//Use ID to set parameter value.
	return true;
}