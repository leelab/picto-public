#include "SlaveEvent.h"
#include "../memleakdetect.h"

using namespace Picto;
SlaveEvent::SlaveEvent()
{
	type = INVALID;
	id = 0;
	dataId = 0;
	value.clear();
}

/*! \brief Sets SlaveEvent data fields for a PROP_VAL_CHANGE event.*/
void SlaveEvent::setAsProp(const qulonglong& data, const int& assetId, const QString& val)
{
	type = PROP_VAL_CHANGE;
	id = assetId;
	value = val;
	dataId = data;
}
/*! \brief Sets SlaveEvent data fields for a INIT_PROP_VAL_CHANGE event.*/
void SlaveEvent::setAsInitProp(const qulonglong& data, const int& assetId, const QString& val)
{
	type = INIT_PROP_VAL_CHANGE;
	id = assetId;
	value = val;
	dataId = data;
}
/*! \brief Sets SlaveEvent data fields for a TRANS_ACTIVATED event.*/
void SlaveEvent::setAsTrans(const qulonglong& data, const int& assetId, bool remoteRunSignal)
{
	type = TRANS_ACTIVATED;
	id = assetId;
	dataId = data;
	//Kind of a hack, but we can use the extra data element to store whether this needs to call the remoteRun functions.
	if (remoteRunSignal)
		value = "1";
	else
		value.clear();
}
