#include "RequiredResult.h"
#include "../memleakdetect.h"
namespace Picto
{

RequiredResult::RequiredResult()
{
	//Required Results can't have scripts, turn them off
	resultScriptFactory_->setMinAssets(0);
	resultScriptFactory_->setMaxAssets(0);
}

QSharedPointer<Asset> RequiredResult::Create()
{
	return QSharedPointer<Asset>(new RequiredResult());
}

}//namespace Picto