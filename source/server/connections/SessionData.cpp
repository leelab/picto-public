#include "SessionData.h"
#include <QMutexLocker>

SessionData::SessionData()
{
}
SessionData::~SessionData()
{

}

QList<QVariantList> SessionData::peekData(int dataType)
{
	QMutexLocker locker(&accessMutex_);
	return readData(dataType,QVariant(),false);
}

void SessionData::moveDataTo(SessionData* receiver,QVariant condition)
{
	copyDataPrivate(receiver,condition,true);
}

void SessionData::copyDataTo(SessionData* receiver,QVariant condition)
{
	copyDataPrivate(receiver,condition,false);
}

void SessionData::addData(int dataType, QVariantList data)
{
	QMutexLocker locker(&accessMutex_);
	QString error;
	bool success = false;
	do{
		success = startDataWrite(&error);
		if(!success)
		{
			qDebug("Failed to initiate data write. Error was: " + error.toLatin1() + "...Reattempting.");
			continue;
		}
		writeData(dataType,data);
		success = endDataWrite(&error);
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

void SessionData::copyDataPrivate(SessionData* receiver,QVariant condition,bool cut)
{
	QMutexLocker locker(&accessMutex_);
	QList<int>dataTypes = readDataTypes();
	if(!dataTypes.size())
		return;

	QString error;
	bool success = false;
	do{
		success = startDataWrite(&error);
		if(!success)
		{
			qDebug("Failed to initiate data write. Error was: " + error.toLatin1() + "...Reattempting.");
			continue;
		}
		foreach(int type,dataTypes)
		{
			QList<QVariantList> dataList = readData(type,condition,cut);
			foreach(QVariantList data,dataList)
			{
				receiver->writeData(type,data);
			}
		}
		success = endDataWrite(&error);
		if(!success)
		{
			qDebug("Failed to finalize data write. Error was: " + error.toLatin1() + "...Reattempting.");
			continue;
		}
	} while(!success);
}