#ifndef _SERIAL_SESSION_DATA_H_
#define _SERIAL_SESSION_DATA_H_

#include <QVariantList>
#include <QMutex>
#include <QSharedPointer>
#include <QPair>

#include "SessionData.h"

/*!	\brief Interface to an object that stores session data
 *
 */

class SerialSessionData : public SessionData
{
public:
	SerialSessionData::SerialSessionData();
	virtual ~SerialSessionData();

	QString takeSerializedData();

protected:

	//Should write the input data to descendant defined data structure
	void writeData(int dataType, QVariantList data);
	QList<int>readDataTypes();
	//Should read all data of the input dataType into a QVector or VariantList
	QList<QVariantList> readData(int dataType,QVariant condition,bool cut=false);
	virtual void eraseEverything();
private:
	QString serialData_;
};

#endif

