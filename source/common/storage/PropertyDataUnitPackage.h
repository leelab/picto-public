#ifndef _PROPERTYDATAUNITPACKAGE_H_
#define _PROPERTYDATAUNITPACKAGE_H_

#include "../common.h"
#include "DataUnit.h"
#include "PropertyDataUnit.h"

#include <QList>
#include <QMap>

namespace Picto {

/*!	\brief Stores a list of PropetyDataUnit objects representing all Property value changes for a single frame
 *
 *	\details This class is meant to be used for transferring the Property value changes over a network.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API PropertyDataUnitPackage : public DataUnit
#else
class PropertyDataUnitPackage : public DataUnit
#endif
{
public:

	PropertyDataUnitPackage();
	void addData(int index, bool initValue, QString value);
	void setActionFrame(qulonglong frameId);
	/*! \brief Removes all PropertyDataUnits from this package.
	 */
	void emptyData() { data_.clear(); };
	/*! \brief Returns the number of PropertyDataUnits in this package.
	 */
	int length() { return data_.length(); }
	/*! \brief Returns the first PropertyDataUnit in this package and removes it from the list.
	 */
	QSharedPointer<PropertyDataUnit> takeFirstDataPoint() { return data_.takeFirst(); };

	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QList<QSharedPointer<PropertyDataUnit>> data_;
};


}; //namespace Picto

#endif
