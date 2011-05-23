#ifndef _BEHAVIORALDATASTORE_H_
#define _BEHAVIORALDATASTORE_H_

#include "../common.h"
#include "DataStore.h"
#include "BehavioralUnitDataStore.h"

#include <QList>
#include <QMap>

namespace Picto {

/*!	\brief Stores behavioral data as x,y,t triplets
 *
 *	The behavioral data store is designed to work with signal channels
 *	to store behavioral data.  The store is used to convert data into
 *  XML fragments for movement across the network.
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API BehavioralDataStore : public DataStore
#else
class BehavioralDataStore : public DataStore
#endif
{
	Q_OBJECT
public:

	BehavioralDataStore();
	void addData(double x, double y, double t);
	void addData(QMap<QString, QList<double>> signalChannelData);

	void emptyData() { data_.clear(); };

	int length() { return data_.length(); }
	QSharedPointer<BehavioralUnitDataStore> takeFirstDataPoint() { return data_.takeFirst(); };

	//Data store functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:

	QList<QSharedPointer<BehavioralUnitDataStore>> data_;
};


}; //namespace Picto

#endif
