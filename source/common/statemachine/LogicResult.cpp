#include "LogicResult.h"
#include "../memleakdetect.h"
namespace Picto
{

LogicResult::LogicResult()
{
	//Required Results can't have scripts, turn them off
	resultScriptFactory_->setMinAssets(0);
	resultScriptFactory_->setMaxAssets(0);
}

QSharedPointer<Asset> LogicResult::Create()
{
	return QSharedPointer<Asset>(new LogicResult());
}

}//namespace Picto