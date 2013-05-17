#include "LogicResult.h"
#include "../memleakdetect.h"
namespace Picto
{

LogicResult::LogicResult()
{
}

QSharedPointer<Asset> LogicResult::Create()
{
	return QSharedPointer<Asset>(new LogicResult());
}

}//namespace Picto