#ifndef _SESSION_DATA_H_
#define _SESSION_DATA_H_

#include <QVariantList>
#include <QReadWriteLock>
#include <QList>

/*!	\brief Interface to an object that stores session data
 *
 */

class SessionData
{
public:
	SessionData::SessionData();
	virtual ~SessionData();

	QList<QVariantList> peekData(int dataType);
	//Copies all data in this SessionData object to another session data object
	//condition variable is some condition that defines which data gets copied.
	//It's meaning is defined in the descendant's implementation of readData().
	void copyDataTo(SessionData* receiver,QVariant condition = QVariant());

	//Moves all data in this SessionData object to another session data object,
	//the data gets deleted from this SessionData object in the process.
	//condition variable is some condition that defines which data gets copied.
	//It's meaning is defined in the descendant's implementation of readData().
	void moveDataTo(SessionData* receiver,QVariant condition = QVariant());

	//Brings this object back into the state it was in when just constructed.
	void clearData();

protected:

	void addData(int dataType, QVariantList data);
	void addData(int dataType, QList<QVariantList> data);
	//Called before writeData.  Should return false if descendant couldn't initialize
	//data write resources.
	virtual bool startDataWrite(QString* error = NULL);
	//Should write the input data to descendant defined data structure
	virtual void writeData(int dataType, QVariantList data) = 0;
	//Called after all writeData's in a batch.  Should return false if descendant 
	//couldn't finalize data write.
	virtual bool endDataWrite(QString* error = NULL);
	virtual QList<int>readDataTypes() = 0;
	//Should read all data of the input dataType into a QVector or VariantList
	//condition input comes from the copyDataTo function and has the same meaning
	//as it does in that location
	virtual QList<QVariantList> readData(int dataType,QVariant condition,bool cut=false) = 0;
	//Should rease all data from the descendant, bringing it to the state that it was in when it was newly
	//constructed.
	virtual void eraseEverything() = 0;

	friend class SessionData;
	QReadWriteLock readWriteLock_;

private:
	bool startDataWriteAndLock(QString* error);
	bool endDataWriteAndUnlock(QString* error);
	void copyDataPrivate(SessionData* receiver,QVariant condition,bool cut);
};

#endif

