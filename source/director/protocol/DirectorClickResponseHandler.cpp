#include "DirectorClickResponseHandler.h"
#include "../../common/engine/propertyTable.h"
#include "../../common/parameter/operatorclickparameter.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

DirectorClickResponseHandler::DirectorClickResponseHandler(QSharedPointer<DirectorStatusManager> statusManager):
ClickResponseHandler(statusManager)
{}

bool DirectorClickResponseHandler::processResponse(QString directive)
{	QStringList xy = directive.split(",");

	OperatorClickParameter::addClick(QPoint(xy.first().toInt(),xy.last().toInt()));
	return true;
}