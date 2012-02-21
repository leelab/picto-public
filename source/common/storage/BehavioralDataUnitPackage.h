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
	void setDescriptor(QString descriptor);
	QString getDescriptor(){return descriptor_;};
	void setChannel(QString channel);
	void setTime(double t){time_ = QString::number(t,'e',6);};
	void setTime(QString t){time_ = t;};
	QString getTime(){return time_;};
	void setResolution(double r){resolution_ = r;};
	double getResolution(){return resolution_;};
	void addData(double x, double y);
	void addData(QMap<QString, QList<double>> signalChannelData);

	void emptyData() { data_.clear(); };

	int length() { return data_.length(); }
	QString getChannel(){return channel_;};
	QSharedPointer<BehavioralDataUnit> takeFirstDataPoint() { return data_.takeFirst(); };
	QSharedPointer<BehavioralDataUnit> peekFirstDataPoint() { return data_.first(); };
	QSharedPointer<BehavioralDataUnit> takeLastDataPoint() { return data_.takeLast(); };
	void clearAllButLastDataPoints();

	QByteArray getDataAsByteArray();

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QString channel_;	//The signal channel from which this data was drawn
	double resolution_;
	QString time_;
	QString descriptor_;
	QList<QSharedPointer<BehavioralDataUnit>> data_;
};


}; //namespace Picto

#endif
