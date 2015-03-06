#ifndef _ALIGN_SESSION_DATA_H_
#define _ALIGN_SESSION_DATA_H_

#include <QVariantList>
#include <QMutex>
#include <QSharedPointer>

#include "SessionData.h"
/*!	\brief Stores Alignment event data for use in the Server's timestamp alignment system.
 *	\details The alignment system frequently goes through the latest alignment events to have arrived
 *	from the Director and Proxy and attemps to match them up.  When they are matched, the alignment
 *	events are marked as such.  This object stores these alignment events for the Director and Proxy.  
 *	It only actually cares about the alignment event's dataId and matched values.  These values 
 *	are tracked throughout internal logic to make sure that a previously written analysis
 *	event that has already been matched is not overwritten with an unmatched value due to things
 *	like a Director resending the event.  The matched column is also tracked to make sure that 
 *	that values are never cut from this object during a readData() call until they are matched.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class AlignSessionData : public SessionData
{
public:
	AlignSessionData::AlignSessionData(int dataType, int matchedColumn);
	virtual ~AlignSessionData();

	void insertData(int dataId, QVariantList data);

protected:
	void writeData(int dataType, QVariantList data);
	QList<int>readDataTypes();
	virtual QList<QVariantList> readData(int dataType,QVariant condition,bool cut=false);
	virtual void eraseEverything();
	QMap<int,QVariantList> dataMap_;	//!< The data structure that holds this objects data.  The map is indexed by dataId and includes Analysis event entries.
	int dataType_;	//!< The single data type that may be used with this object.  It is the one entered into the constructor.
private:
	int matchedCol_;
};

#endif

