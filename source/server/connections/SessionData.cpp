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

/*! \brief Returns a list of all data stored in this object of the input dataType.
*/
QList<QVariantList> SessionData::peekData(int dataType)
{
	QReadLocker locker(&readWriteLock_);
	return readData(dataType,QVariant(),false);
}

/*! \brief Moves all data in this SessionData object to another session data object,
 *	@param receiver The SessionData object to which data will be copied.
 *	@param condition Some condition that defines which data gets copied.  It's meaning is defined in the readData() implementation.
 *	\details Internally, this forwards calls to copyDataPrivate() with "true" for the cut condition.
 */
void SessionData::moveDataTo(SessionData* receiver,QVariant condition)
{
	copyDataPrivate(receiver,condition,true);
}

/*! \brief Brings this object back into the state it was in when just constructed.*/
void SessionData::clearData()
{
	QWriteLocker locker(&readWriteLock_);
	eraseEverything();
}

/*! \brief Copies all data in this SessionData object to another SessionData object.
 *	@param receiver The SessionData object to which data will be copied.
 *	@param condition Some condition that defines which data gets copied.  It's meaning is defined in the readData() implementation.
 *	\details Internally, this forwards calls to copyDataPrivate() with "false" for the cut condition.
 */
void SessionData::copyDataTo(SessionData* receiver,QVariant condition)
{
	copyDataPrivate(receiver,condition,false);
}

/*! \brief Adds a single data entry to this SessionData object of the input type.
 *	@param data A VariantList containing the data entry in a way that should be 
 *	understandable to anyone working with the input dataType.
 *	\details Internally, this just calls the addData(int,QList<QVariantList>) with a 
 *	list that has only a single QVariantList item.
 */
void SessionData::addData(int dataType, QVariantList data)
{
	addData(dataType,QList<QVariantList>() << data);
}

/*! \brief Adds the input QList of QVariantList data entries of the input type to this SessionData object.
 *	@param data A QList of VariantList's each of which contains a data entry setup in a way that should be 
 *	understandable to anyone working with the input dataType.
 *	\details Internally, this uses startDataWriteAndLock() to lock multithreaded access to this
 *	object and call startDataWrite(), then it calls writeData() one by one for all data entries,
 *	then calls endDataWriteAndUnlock() to call endDataWrite() and unlock multithreaded access to 
 *	this object.
 */
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

/*! \brief Called before the first of a single or batched up calls to writeData().  
 *	\details Should return false if resources needed for data write couldn't be initialized.
 *	\note There is a call to endDataWrite() for every call to startDataWrite().
 */
bool SessionData::startDataWrite(QString*)
{
	return true;
}

/*! \brief Called after a completed batch of writeData() calls to finalize the data write.  Should return false if the data write could not be finalized.
 *	\note There is a call to endDataWrite() for every call to startDataWrite().
 */
bool SessionData::endDataWrite(QString*)
{
	return true;
}

/*! \brief Locks this object for writing and calls startDataWrite().
 *	\details Returns the result of startDataWrite().  error is set
 *	from that function as well.
 */
bool SessionData::startDataWriteAndLock(QString* error)
{
	readWriteLock_.lockForWrite();
	return startDataWrite(error);
}

/*! \brief Calls endDataWrite() and unlocks this object after writing.
 *	\details Returns the result of endDataWrite().  error is set
 *	from that function as well.
 */
bool SessionData::endDataWriteAndUnlock(QString* error)
{
	bool returnVal = endDataWrite(error);
	readWriteLock_.unlock();
	return returnVal;
}

/*! \brief Reads all data from this object that meets the input condition and writes it
 *	to the input reciever.
 *	@param cut If true, all data that is read from this object by this function will be
 *		deleted from this object (this happens in readData()).
 *	\details This gets all of this object's data types, then passes the condition and cut inputs
 *	into readData() for each data type, and writes that data to the input reciever in a manner
 *	similar to that used in addData(int, QList<QVariantList>).
 */
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