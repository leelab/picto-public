#include "SlaveEvent.h"
#include "../memleakdetect.h"

using namespace Picto;
SlaveEvent::SlaveEvent()
{
	type = INVALID;
	id = 0;
	value.clear();
}

/*! \brief Sets SlaveEvent data fields for a PROP_VAL_CHANGE event.*/
void SlaveEvent::setAsProp(const int& assetId, const QString& val)
{
	type = PROP_VAL_CHANGE;
	id = assetId;
	value = val;
}
/*! \brief Sets SlaveEvent data fields for a INIT_PROP_VAL_CHANGE event.*/
void SlaveEvent::setAsInitProp(const int& assetId, const QString& val)
{
	type = INIT_PROP_VAL_CHANGE;
	id = assetId;
	value = val;
}
/*! \brief Sets SlaveEvent data fields for a TRANS_ACTIVATED event.*/
void SlaveEvent::setAsTrans(const int& assetId)
{
	type = TRANS_ACTIVATED;
	id = assetId;
}