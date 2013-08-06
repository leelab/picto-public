#include <QTime>
#include <QReadLocker>
#include <QWriteLocker>
#include <QVector>
#include "SessionData.h"

SessionData::SessionData()
{
}
SessionData::~SessionData()
{

}

QList<QVariantList> SessionData::peekData(int dataType)
{
	QReadLocker locker(&readWriteLock_);
	return readData(dataType,QVariant(),false);
}

void SessionData::moveDataTo(SessionData* receiver,QVariant condition)
{
	copyDataPrivate(receiver,condition,true);
}

void SessionData::clearData()
{
	QWriteLocker locker(&readWriteLock_);
	eraseEverything();
}

void SessionData::copyDataTo(SessionData* receiver,QVariant condition)
{
	copyDataPrivate(receiver,condition,false);
}

void SessionData::addData(int dataType, QVariantList data)
{
	addData(dataType,QList<QVariantList>() << data);
}

void SessionData::addData(int dataType, QList<QVariantList> data)
{
	QString error;
	bool success = false;
	do{
		success = startDataWriteAndLock(&error);
		if(!success)
		{
			qDebug("Failed to initiate data write. Error was: " + error.toLatin1() + "...Reattempting.");
			continue;
		}
		foreach(QVariantList dataItem, data)
		{
			writeData(dataType,dataItem);
		}
		success = endDataWriteAndUnlock(&error);
		if(!success)
		{
			qDebug("Failed to finalize data write. Error was: " + error.toLatin1() + "...Reattempting.");
			continue;
		}
	} while(!success);
}

bool SessionData::startDataWrite(QString*)
{
	return true;
}

bool SessionData::endDataWrite(QString*)
{
	return true;
}

bool SessionData::startDataWriteAndLock(QString* error)
{
	readWriteLock_.lockForWrite();
	return startDataWrite(error);
}

bool SessionData::endDataWriteAndUnlock(QString* error)
{
	bool returnVal = endDataWrite(error);
	readWriteLock_.unlock();
	return returnVal;
}

void SessionData::copyDataPrivate(SessionData* receiver,QVariant condition,bool cut)
{
	Q_ASSERT(receiver != this);
	readWriteLock_.lockForRead();
	QVector<int>dataTypes = readDataTypes().toVector();	//Turning the list into a vector forces a deep copy to occur.  Otherwise there are a lot of multithreading slow down issues when iterating through the list
	readWriteLock_.unlock();
	if(!dataTypes.size())
	{
		return;
	}

	QString error;
	bool success = false;
	do{
		success = receiver->startDataWriteAndLock(&error);
		if(!success)
		{
			qDebug("Failed to initiate data write. Error was: " + error.toLatin1() + "...Reattempting.");
			continue;
		}
		foreach(int type,dataTypes)
		{
			if(cut)
				readWriteLock_.lockForWrite();
			else
				readWriteLock_.lockForRead();
			QVector<QVariantList> dataList = readData(type,condition,cut).toVector();	//Turning the list into a vector forces a deep copy to occur.  Otherwise there are a lot of multithreading slow down issues when iterating through the list
			readWriteLock_.unlock();
			foreach(QVariantList data,dataList)
			{
				receiver->writeData(type,data);
			}
		}
		success = receiver->endDataWriteAndUnlock(&error);
		if(!success)
		{
			qDebug("Failed to finalize data write. Error was: " + error.toLatin1() + "...Reattempting.");
			continue;
		}
	} while(!success);
}