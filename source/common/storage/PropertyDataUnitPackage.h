#ifndef _PROPERTYDATAUNITPACKAGE_H_
#define _PROPERTYDATAUNITPACKAGE_H_

#include "../common.h"
#include "DataUnit.h"
#include "PropertyDataUnit.h"

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
	class PICTOLIB_API PropertyDataUnitPackage : public DataUnit
#else
class PropertyDataUnitPackage : public DataUnit
#endif
{
public:

	PropertyDataUnitPackage();
	void addData(int index, QString path, QString value, double time);
	void addData(int index, QString path, QString value, QString time);

	void setAllTimestamps(double newTime);

	void emptyData() { data_.clear(); };

	int length() { return data_.length(); }
	QSharedPointer<PropertyDataUnit> takeFirstDataPoint() { return data_.takeFirst(); };

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:

	QList<QSharedPointer<PropertyDataUnit>> data_;
};


}; //namespace Picto

#endif
