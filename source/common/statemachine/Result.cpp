#include "Result.h"
#include "../memleakdetect.h"
namespace Picto
{

Result::Result()
{
	
}

Result::Result(QString name)
{
	
	initializePropertiesToDefaults();
	//Even though the name is edited, we set the result as unedited because we want 
	//this to serialize out with its name, but only if something else about it changes.
	setName(name);
	setUnedited();
}

QSharedPointer<Asset> Result::Create()
{
	return QSharedPointer<Asset>(new Result());
}

void Result::postDeserialize()
{
	ScriptableContainer::postDeserialize();
}


bool Result::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptableContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto