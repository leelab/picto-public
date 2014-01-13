#ifndef _SESSION_DATA_H_
#define _SESSION_DATA_H_

#include <QVariantList>
#include <QReadWriteLock>
#include <QList>

/*!	\brief The base class or objects that stores session data.
 *	\details There is some history to this class.  Originally, we used to create two databases
 *	on the server, one file system database, and another very similiar database that was stored
 *	in RAM.  We then periodically wrote the cache database out to the file database.  When we found
 *	that the original system required a lot of complex and costly data access for workstations to join
 *	running sessions, we also added a lookup table with the current values of all Properties and other
 *	 data.  This allowed workstations to join up with running sessions more quickly.  At one point 
 *	though, in the process of upgrading our Qt version, we found that we were suddenly getting lots of 
 *	errors that were crashing the server.  With some research we discovered that cached Sqlite databases 
 *	were only supposed to be accessed by a single thread, and this was causing the crash since the sockets
 *	handling Director and Proxy data, along with the socket handling Workstation data access all had
 *	their own threads and were accessing this same cached database.  
 *
 *	This class was created to solve that problem.  SessionData objects support multi-threaded access, and 
 *	can easily write data to one another using the copyDataTo() or moveDataTo() functions.  Essentially, 
 *	the entire SessionData system is built to replace the in memory Sqlite database that doesn't support
 *	multithreaded access.  By understanding the use case, we can get away without all of the SQL support
 *	that we didn't actually need and simply handle what needs to be done here in our server.  By creating
 *	a StoredSessionData SessionData class, we have even been able to simplify our SQL file interface by hiding
 *	it all inside that class and writing to SQL by simply copying data from a CachedSessionData object
 *	to a StoredSessionData object.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class SessionData
{
public:
	SessionData::SessionData();
	virtual ~SessionData();

	QList<QVariantList> peekData(int dataType);
	void copyDataTo(SessionData* receiver,QVariant condition = QVariant());
	void moveDataTo(SessionData* receiver,QVariant condition = QVariant());
	void clearData();

protected:

	void addData(int dataType, QVariantList data);
	void addData(int dataType, QList<QVariantList> data);
	virtual bool startDataWrite(QString* error = NULL);
	/*! \brief Write the data of the input dataType to this object.*/
	virtual void writeData(int dataType, QVariantList data) = 0;
	virtual bool endDataWrite(QString* error = NULL);
	/*! \brief Returns a list of integers representing types of data handled by this object.
	 *	\details For example, if this SessionData handles both Transition traversal and Property
	 *	value change data, Transition traversal data might be data type 1 and Property value change
	 *	data might be data type 2 such that this functio would return {1,2}.
	 */
	virtual QList<int>readDataTypes() = 0;
	/*! \brief Reads all data of the input dataType into a QVector or VariantList.
	 *	\details The meaning of the condition input is a function of the type of this SessionData
	 *	class.
	 */
	virtual QList<QVariantList> readData(int dataType,QVariant condition,bool cut=false) = 0;
	//Should rease all data from the descendant, bringing it to the state that it was in when it was newly
	//constructed.
	virtual void eraseEverything() = 0;

	friend class SessionData;	//This is unnecessary.  Remove it.
	QReadWriteLock readWriteLock_;	//!< A Read/Write mutex for handling reads and write from multiple threads.

private:
	bool startDataWriteAndLock(QString* error);
	bool endDataWriteAndUnlock(QString* error);
	void copyDataPrivate(SessionData* receiver,QVariant condition,bool cut);
};

#endif

