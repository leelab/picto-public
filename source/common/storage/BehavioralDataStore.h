/*!	\brief Stores behavioral data as x,y,t triplets
 *
 *	The behavioral data store is designed to work with signal channels
 *	to store behavioral data.  The store is used to convert data into
 *  XML fragments for movement across the network.
 *
 *	The id is used so that we can keep track of different pieces of 
 *	behavioral data (e.g. matching up commands with responses)
 */

#ifndef _BEHAVIORALDATASTORE_H_
#define _BEHAVIORALDATASTORE_H_

#include "../common.h"
#include "DataStore.h"

#include <QList>
#include <QMap>

namespace Picto {

#if defined WIN32 || defined WINCE
	class PICTOLIB_API BehavioralDataStore : public DataStore
#else
class BehavioralDataStore : public DataStore
#endif
{
public:
	typedef struct BehavioralDataPoint_st
	{
		double x;
		double y;
		double t;
	} BehavioralDataPoint;

	BehavioralDataStore();
	void addData(double x, double y, double t);
	void addData(QMap<QString, QList<double>> signalChannelData);

	void emptyData() { data_.clear(); };

	//Data store functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	int length() { return data_.length(); }
	BehavioralDataPoint takeFirstDataPoint() { return data_.takeFirst(); };
private:

	QList<BehavioralDataPoint> data_;
};


}; //namespace Picto

#endif
