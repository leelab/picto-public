#include <QMutexLocker>
#include <QStringList>
#include <QThread>
#include <QSqlDatabase>
#include <QSqlError>
#include <qlogging.h>
#include "StoredSessionData.h"

/*! \brief Creates a new StoredSessionData object that will put Session data in a Sqlite database
 *	in the sessionPath with the databaseFileName name.
 */
StoredSessionData::StoredSessionData(QString sessionPath,QString databaseFileName)
{
	QSqlDatabase baseSessionDb = QSqlDatabase::addDatabase("QSQLITE",databaseFileName);
	baseSessionDbConnection_ = databaseFileName;
	openDatabaseConnections_.append(databaseFileName);
	baseSessionDb.setDatabaseName(sessionPath + "/" + databaseFileName);
	baseSessionDb.open();
	Q_ASSERT(baseSessionDb.isOpen());
	QSqlQuery query(baseSessionDb);
	query_.clear();

	//Set the synchronous setting: 
	//FULL is fully protected from application crash and system failure - This is the Sqlite default
	//NORMAL is fully protected from application crash and most system failure case won't corrupt
	//			the database.  In practice, a catastrophic disk or hardware fault is more likely to
	//			occur.
	//OFF is fully protected from application crash.  System failures could corrupt data.
	bool success = query.exec("PRAGMA synchronous = OFF");
	Q_ASSERT_X(success,"StoredSessionData::StoredSessionData","Error: Failed to initialize database: "+query.lastError().text().toLatin1());
	//The DELETE (default) journaling mode is the normal behavior. In the DELETE mode, the rollback 
	//journal is deleted at the conclusion of each transaction. Indeed, the delete operation is the 
	//action that causes the transaction to commit. (See the documented titled Atomic Commit In SQLite 
	//for additional detail.)
	//The WAL journaling mode uses a write-ahead log instead of a rollback journal to implement 
	//transactions. The WAL journaling mode is persistent; after being set it stays in effect across 
	//multiple database connections and after closing and reopening the database. A database in WAL 
	//journaling mode can only be accessed by SQLite version 3.7.0 or later. This mode can be significantly
	//faster than DELETE in cases where there are significantly more writes than reads.  This mode uses
	//three files, which makes it distastful for using sqlite as a file type.
	success = query.exec("PRAGMA journal_mode = DELETE");
	Q_ASSERT_X(success,"StoredSessionData::StoredSessionData","Error: Failed to initialize database: "+query.lastError().text().toLatin1());
	//Set the temp_store setting: 
	//The temp_store values specifies the type of database back-end to use for temporary files. 
	//The choices are DEFAULT (0), FILE (1), and MEMORY (2). The use of a memory database for 
	//temporary tables can produce signifigant savings. DEFAULT specifies the compiled-in default, 
	//which is FILE unless the source has been modified.
	success = query.exec("PRAGMA temp_store = 2");
	Q_ASSERT_X(success,"StoredSessionData::StoredSessionData","Error: Failed to initialize database: "+query.lastError().text().toLatin1());
}

/*! \brief Called when the StoredSessionData object is deleted.  Closes all Sqlite database resources.
 */
StoredSessionData::~StoredSessionData()
{
	//Close all database connections
	foreach(QString connectionName, openDatabaseConnections_)
	{
		QSqlDatabase connection = QSqlDatabase::database(connectionName);
		if(connection.isValid())
			connection.close();
	}
	
	if(query_)
	{
		query_.clear();
	}
	//Remove all database connections
	foreach(QString connectionName, openDatabaseConnections_)
	{
		if(QSqlDatabase::contains(connectionName))
			QSqlDatabase::removeDatabase(connectionName);
	}
}

/*! \brief Sets the SessionId of the session that will use this StoredSessionData object.
 *	\details The SessionId is used in generating uniquely named database connections when 
 *	multiple threads access this database.
 *	\sa getSessionDb()
 */
void StoredSessionData::setSessionId(QUuid sessionId)
{
	sessionId_ = sessionId;
}

/*! \brief Stores information about the SQL table that should be used to store data for different data types.
 *	@param dataType An integer signifying the type of data to which these settings will apply.
 *	@param tableName The name of the table that will be used to store data for this dataType.
 *	@param columnNames A comma separated string including the names of the columns that will be used to 
 *		store data for this dataType (put spaces on both ends of the string, like " dataId,timestamp,value ").
 *	@param columnTypes A comma separated string including the Sqlite types of the columns that will be used to 
 *		store data for this dataType (put spaces on both ends of the string, 
 *		like " INTEGER UNIQUE ON CONFLICT REPLACE,REAL,TEXT ").
 *	@param indexedColumns A comma separated string including the names of the columns that will be indexed for fast
 *		lookup by the Sqlite system for this dataType (put spaces on both ends of the string, like " dataId,timestamp ").
 *	\details This function actually creates the necessary table in the Sql database if it doesn't already exist.
 *	\note The indeces are not built when you call this function.  Once indeces are built, additions to the SQL table
 *	can take significantly longer.  For this reason, we only want to add indeces at the end of the session.  To do that
 *	you must use buildTableIndeces().
 */
void StoredSessionData::setTableInfo(int dataType,QString tableName,QString columnNames,QString columnTypes,QString indexedColumns)
{
	Q_ASSERT(!tableByType_.contains(dataType));
	QStringList placeHolderList = columnNames.split(",",QString::SkipEmptyParts);
	for(int i=0;i<placeHolderList.size();i++)
	{
		placeHolderList[i] = "?";
	}
	QString placeHolderString = placeHolderList.join(",");
	tableByType_[dataType] = TableInfo(tableName,columnNames,columnTypes,indexedColumns,placeHolderString);

	QSqlQuery query( QSqlDatabase::database(baseSessionDbConnection_));
	//If table doesn't exist in file, create it here
	QStringList tableColumns = columnNames.split(",");
	QStringList tableColumnTypes = columnTypes.split(",");
	QString columnDefString;
	int colLength = tableColumns.size();
	int typeLength = tableColumnTypes.size();
	for(int i=0;(i<colLength || i<typeLength);i++)
	{
		columnDefString.append(QString(",") + ((i<colLength)?tableColumns[i]:"") + " " + ((i<colLength)?tableColumnTypes[i]:"")); 
	}
	QString createTableString = "CREATE TABLE IF NOT EXISTS "+tableName+" (id  INTEGER PRIMARY KEY"+columnDefString+");";
	bool success = query.exec(createTableString);
	(void*)success;
	Q_ASSERT_X(success,"StoredSessionData::setTableInfo","Error: Failed to create table: " + tableName.toLatin1() + ", Error: " +query.lastError().text().toLatin1());
}

/*! \brief Builds all table indeces that were requested in setTableInfo().
 *	\details This should typically be done only at the end of a session.  See setTableInfo() documentation for more details.
 */
void StoredSessionData::buildTableIndeces()
{
	QSqlQuery query(getSessionDb());
	//If indeces are needed and they're not in file, create them here
	foreach(TableInfo table,tableByType_.values())
	{
		QStringList tableIndeces = table.indexedColumns.split(",",QString::SkipEmptyParts);
		foreach(QString index,tableIndeces)
		{
			bool success = query.exec("CREATE INDEX IF NOT EXISTS "+table.name+"_"+index.trimmed()+"_index ON "+table.name+"("+index+");");
			(void*)success;
			Q_ASSERT_X(success,"StoredSessionData::buildTableIndeces","Error: Failed to create index: " + index.trimmed().toLatin1() + " for table: " + table.name.toLatin1() + ", Error: " +query.lastError().text().toLatin1());
		}
	}
}

/*! \brief Inserts the input data of the input dataType into this object.
 *	\details Internally, this just sends the inputs to addData().
 */
void StoredSessionData::insertData(int dataType, QVariantList data)
{
	addData(dataType,data);
}

/*! \brief Implements SessionData::startDataWrite() to setup the SqlQuery that will be used and start
 *	a SQL transaction.
 *	\details We put all batched writeData() calls into a single transaction for significant runtime
 *	savings.
 */
bool StoredSessionData::startDataWrite(QString* error)
{
	QSqlDatabase sessionDb = getSessionDb();
	if(!query_.isNull())
	{
		Q_ASSERT(query_.isNull());
	}
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(getSessionDb()));
	bool success = sessionDb.transaction();
	if(!success)
	{
		query_.clear();
		(*error) = sessionDb.lastError().text();
		return false;
	}
	return true;
}

/*! \brief Writes the input data to the SQL database by creating an SQL query according to the information 
 *	entered in setTableInfo() for the input dataType.
 *	\details writeData() calls are always in between startDataWrite() and endDataWrite() calls.  For the
 *	purpose of runtime savings, these surrounding functions setup and commit an SQL transaction such that
 *	no actual writing of data to the SQL database is occuring during this function.  It only occurs when
 *	endDataWrite() is called.
 *	\sa startDataWrite(), endDataWrite()
 */
void StoredSessionData::writeData(int dataType, QVariantList data)
{
	Q_ASSERT(tableByType_.contains(dataType));
	TableInfo tableInfo = tableByType_[dataType];
	QString queryString = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableInfo.name).arg(tableInfo.colNames).arg(tableInfo.placeholderString);
	query_->prepare(queryString);
	foreach(QVariant value,data)
	{
		query_->addBindValue(value);
	}
	bool success = query_->exec();
	(void*)success;
	Q_ASSERT_X(success,"StoredSessionData::writeData","Error: "+query_->lastError().text().toLatin1());
}

/*! \brief Implements SessionData::endDataWrite() to commit the SQL transaction that was opened in 
 *	startDataWrite() and get rid of the SqlQuery.
 *	\details Since we put all batched writeData() calls into a single transaction, the actual writing
 *	of the data to the underlying database doesn't happen until this function is called.
 *	\sa startDataWrite(), writeData()
 */
bool StoredSessionData::endDataWrite(QString* error)
{
	QSqlDatabase sessionDb = getSessionDb();
	bool success = sessionDb.commit();
	query_.clear();
	if(!success)
	{
		(*error) = sessionDb.lastError().text();
		sessionDb.rollback();
		return false;
	}
	return true;
}

/*! \brief Implements SessionData::readDataTypes() to return all the dataTypes that have
 *	had their SQL table information set up in setTableInfo().
 */
QList<int> StoredSessionData::readDataTypes()
{
	return tableByType_.keys();
}

/*!\ brief Implements SessionData:readData() to read out all data from the underlying SQL database
 *	for the input dataType.
 *	\details Each value will be set up by retrieving data from the table for the input dataType in 
 *	the order that the columns were set up in setTableInfo() and putting it all together into a 
 *	QVariantList.  Each QVariantList is appended to a QList<QVariantList> and returned.
 * 
 *	\note The condition and cut parameters are not supported by this class.
 */
QList<QVariantList> StoredSessionData::readData(int dataType,QVariant condition,bool cut)
{
	(void*)cut;
	Q_ASSERT((condition == QVariant())	 && !cut);//These options are not supported
	Q_ASSERT(tableByType_.contains(dataType));
	QSqlDatabase sessionDb = getSessionDb();
	QSqlQuery query(getSessionDb());
	TableInfo tableInfo = tableByType_[dataType];
	int numColumns = tableInfo.colNames.split(",",QString::SkipEmptyParts).size();
	QList<QVariantList> returnVal;
	
	bool success = query.exec(QString("SELECT %1 FROM %2").arg(tableInfo.colNames).arg(tableInfo.name));
	Q_ASSERT_X(success,"StoredSessionData::writeData","Error: "+query.lastError().text().toLatin1());
	if(!success)
	{
		return returnVal;
	}

	while(query.next())
	{
		QVariantList returnRow;
		for(int i=0;i<numColumns;i++)
			returnRow.append(query.value(i));
		returnVal.append(returnRow);
	}
	success = query.exec();	//What is this doing here?
	return returnVal;
}

/*! \brief Implements SessionData::eraseEverything() to do nothing and trigger an
 *	assertion since this function should never be called on objects of this class.
 */
void StoredSessionData::eraseEverything()
{
	Q_ASSERT(false);
	//The stored session data object currently does not support clearing data.
}

/*! \brief Generates a thread-specific connection to the Session database
 *
 *	The session database has threading issues because of the following:
 *		1. The session database may be accessed from different threads 
 *		   (remember, the commands coming into the server are processed
 *		   by different threads)
 *		2. File Databases can be used from multiple threads, but QSqlDatabase
 *			database connections cannot be shared across threads.
 *	The current solution to these problems is to create a unique database
 *	connection for each thread.  If you can figure out a cleaner way to do this
 *	go for it.
 */
QSqlDatabase StoredSessionData::getSessionDb()
{
	//If the session ID hasn't been set yet, we haven't branched into multiple
	//threads yet.  Just return the base session database.
	if(sessionId_.isNull())
		return QSqlDatabase::database(baseSessionDbConnection_);
	QSqlDatabase sessionDb;
	QString connectionName = QString("SessionDatabase_%1_%2")
					.arg(sessionId_.toString())
					.arg((int)QThread::currentThreadId());

	
	//If we already have a connection open in this thread, use it, 
	//otherwise, clone a new connection
	if(QSqlDatabase::contains(connectionName))
	{
		sessionDb = QSqlDatabase::database(connectionName);
	}
	else
	{
		openDatabaseConnections_.append(connectionName);
		sessionDb = QSqlDatabase::cloneDatabase(QSqlDatabase::database(baseSessionDbConnection_),connectionName);
		sessionDb.open();
	}

	Q_ASSERT(sessionDb.isOpen());

	return sessionDb;
}