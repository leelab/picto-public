#ifndef _SERIAL_SESSION_DATA_H_
#define _SERIAL_SESSION_DATA_H_

#include <QVariantList>
#include <QMutex>
#include <QSharedPointer>
#include <QPair>

#include "SessionData.h"

/*!	\brief Interfaces with CurrentSessionData objects to convert their data into strings of Serial XML data.
 *	\details This is a simple SessionData object that pretty much just takes in data from CurrentSessionData
 *	objects through copyDataTo() or moveDataTo() functions, takes its Serial XML data and appends it
 *	to a string that can be retrieved from takeSerializedData().
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class SerialSessionData : public SessionData
{
public:
	SerialSessionData::SerialSessionData();
	virtual ~SerialSessionData();

	QString takeSerializedData();

protected:

	void writeData(int dataType, QVariantList data);
	QList<int>readDataTypes();
	QList<QVariantList> readData(int dataType,QVariant condition,bool cut=false);
	virtual void eraseEverything();
private:
	QString serialData_;
};

#endif

