#include "SerialSessionData.h"
#include <QWriteLocker>

SerialSessionData::SerialSessionData()
{

}
SerialSessionData::~SerialSessionData()
{

}

/*! \brief Once data has been written to this object, takeSerializedData() returns
 *	this data in an XML serialized form as a single string.  Once this is called, 
 *	all stored data is removed from this object.
 */
QString SerialSessionData::takeSerializedData()
{
	QWriteLocker locker(&readWriteLock_);
	QString returnVal = serialData_;
	serialData_.clear();
	return returnVal;
}

/*! \brief Implements SessionData::writeData() to write the input data to this object.
 *	\details Since this object is meant to be used only with CurrentSessionData, we
 *	know that the data in index 3 of the input data entry will be the serialized 
 *	xml data for the entry.  This allows us to simply take data[3] and append it
 *	to the string that will be returned when takeSerializedData() is called.
 */
void SerialSessionData::writeData(int, QVariantList data)
{
	Q_ASSERT(data.size() == 4);
	if (data.size() == 4)
	{
		serialData_.append(data[3].toString());
	}
}

/*! \brief Implements SessionData::readDataTypes() to return an empty list because SerialSessionData can't be moved to another 
 *	SessionData object.
 */
QList<int>SerialSessionData::readDataTypes()
{
	return QList<int>();
}

/*! \brief Implements SessionData::readData() to return an empty list since SerialSessionData cannot be moved to another SessionData
 *	object.
 */
QList<QVariantList> SerialSessionData::readData(int,QVariant,bool)
{
	//SerialSessionData cant be moved to another SessionData object
	return QList<QVariantList>();
}

void SerialSessionData::eraseEverything()
{
	serialData_.clear();
}