#include "LogicResult.h"
#include "../memleakdetect.h"
namespace Picto
{

LogicResult::LogicResult()
{
}
/*! \brief Creates a new LogicResult object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> LogicResult::Create()
{
	return QSharedPointer<Asset>(new LogicResult());
}

}//namespace Picto