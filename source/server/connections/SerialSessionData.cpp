#include "SerialSessionData.h"
#include <QWriteLocker>

SerialSessionData::SerialSessionData()
{

}
SerialSessionData::~SerialSessionData()
{

}

QString SerialSessionData::takeSerializedData()
{
	QWriteLocker locker(&readWriteLock_);
	QString returnVal = serialData_;
	serialData_.clear();
	return returnVal;
}

void SerialSessionData::writeData(int, QVariantList data)
{
	Q_ASSERT(data.size() == 4);
	serialData_.append(data[3].toString());
}

//SerialSessionData cant be moved to another SessionData object
QList<int>SerialSessionData::readDataTypes()
{
	return QList<int>();
}

//In this function, condition is a timestamp after which all data is read
QList<QVariantList> SerialSessionData::readData(int,QVariant,bool)
{
	//SerialSessionData cant be moved to another SessionData object
	return QList<QVariantList>();
}

void SerialSessionData::eraseEverything()
{
	serialData_.clear();
}