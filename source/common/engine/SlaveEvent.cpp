#include "SlaveEvent.h"
#include "../memleakdetect.h"

using namespace Picto;
SlaveEvent::SlaveEvent()
{
	type = INVALID;
	id = 0;
	value.clear();
}

void SlaveEvent::setAsProp(const int& assetId, const QString& val)
{
	type = PROP_VAL_CHANGE;
	id = assetId;
	value = val;
}
void SlaveEvent::setAsInitProp(const int& assetId, const QString& val)
{
	type = INIT_PROP_VAL_CHANGE;
	id = assetId;
	value = val;
}
void SlaveEvent::setAsTrans(const int& assetId)
{
	type = TRANS_ACTIVATED;
	id = assetId;
}