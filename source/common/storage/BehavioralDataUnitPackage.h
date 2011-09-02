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
	void addData(double x, double y, double t);
	void addData(double x, double y, QString t);
	void addData(QMap<QString, QList<double>> signalChannelData);

	void emptyData() { data_.clear(); };

	int length() { return data_.length(); }
	QSharedPointer<BehavioralDataUnit> takeFirstDataPoint() { return data_.takeFirst(); };

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:

	QList<QSharedPointer<BehavioralDataUnit>> data_;
};


}; //namespace Picto

#endif
