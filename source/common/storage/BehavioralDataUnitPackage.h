#ifndef _BEHAVIORALDATAUNITPACKAGE_H_
#define _BEHAVIORALDATAUNITPACKAGE_H_

#include "../common.h"
#include "DataUnit.h"
#include "BehavioralDataUnit.h"

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
	class PICTOLIB_API BehavioralDataUnitPackage : public DataUnit
#else
class BehavioralDataUnitPackage : public DataUnit
#endif
{
public:

	BehavioralDataUnitPackage();
	void setChannel(QString channel);
	void addData(double x, double y, double t);
	void addData(double x, double y, QString t);
	void addData(QMap<QString, QList<double>> signalChannelData);

	void emptyData() { data_.clear(); };

	int length() { return data_.length(); }
	QString getChannel(){return channel_;};
	QSharedPointer<BehavioralDataUnit> takeFirstDataPoint() { return data_.takeFirst(); };
	QSharedPointer<BehavioralDataUnit> peekFirstDataPoint() { return data_.first(); };
	QSharedPointer<BehavioralDataUnit> takeLastDataPoint() { return data_.takeLast(); };

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QString channel_;	//The signal channel from which this data was drawn
	QList<QSharedPointer<BehavioralDataUnit>> data_;
};


}; //namespace Picto

#endif
