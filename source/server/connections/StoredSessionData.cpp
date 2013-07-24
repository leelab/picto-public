#include <QMutexLocker>
#include <QStringList>
#include <QThread>
#include <QSqlError>
#include "StoredSessionData.h"

StoredSessionData::StoredSessionData(QString sessionPath,QString databaseFileName)
{
	baseSessionDbConnection_ = QSqlDatabase::addDatabase("QSQLITE",databaseFileName);
	openDatabaseConnections_.append(databaseFileName);
	baseSessionDbConnection_.setDatabaseName(sessionPath + "/" + databaseFileName);
	baseSessionDbConnection_.open();
}
StoredSessionData::~StoredSessionData()
{
	foreach(QString connectionName, openDatabaseConnections_)
	{
		QSqlDatabase connection = QSqlDatabase::database(connectionName);
		if(connection.isValid())
			connection.close();
		if(QSqlDatabase::contains(connectionName))
			QSqlDatabase::removeDatabase(connectionName);
	}
}

void StoredSessionData::setSessionId(QUuid sessionId)
{
	sessionId_ = sessionId;
}

void StoredSessionData::setTableInfo(int dataType,QString tableName,QString columnNames,QString columnTypes)
{
	Q_ASSERT(!tableByType_.contains(dataType));
	QStringList placeHolderList = columnNames.split(",",QString::SkipEmptyParts);
	for(int i=0;i<placeHolderList.size();i++)
	{
		placeHolderList[i] = "?";
	}
	QString placeHolderString = placeHolderList.join(",");
	tableByType_[dataType] = TableInfo(tableName,columnNames,columnTypes,placeHolderString);
}

bool StoredSessionData::startDataWrite(QString* error)
{
	QSqlDatabase sessionDb = getSessionDb();
	Q_ASSERT(query_.isNull());
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

void StoredSessionData::writeData(int dataType, QVariantList data)
{
	Q_ASSERT(tableByType_.contains(dataType));
	TableInfo tableInfo = tableByType_[dataType];
	query_->prepare(QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableInfo.name).arg(tableInfo.colNames).arg(tableInfo.placeholderString));
	foreach(QVariant value,data)
	{
		query_->addBindValue(value);
	}
	bool success = query_->exec();
	Q_ASSERT_X(success,"StoredSessionData::writeData","Error: "+query_->lastError().text().toLatin1());
}
bool StoredSessionData::endDataWrite(QString* error)
{
	QSqlDatabase sessionDb = getSessionDb();
	query_.clear();
	bool success = sessionDb.commit();
	if(!success)
	{
		(*error) = sessionDb.lastError().text();
		sessionDb.rollback();
		return false;
	}
	return true;
}
QList<int> StoredSessionData::readDataTypes()
{
	return tableByType_.keys();
}

QList<QVariantList> StoredSessionData::readData(int dataType,QVariant condition,bool cut)
{
	Q_ASSERT((condition == QVariant())	 && !cut);//These options are not supported
	Q_ASSERT(tableByType_.contains(dataType));
	QSqlDatabase sessionDb = getSessionDb();
	Q_ASSERT(query_.isNull());
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(getSessionDb()));
	TableInfo tableInfo = tableByType_[dataType];
	int numColumns = tableInfo.colNames.split(",",QString::SkipEmptyParts).size();
	QList<QVariantList> returnVal;
	
	bool success = query_->exec(QString("SELECT %1 FROM %2").arg(tableInfo.colNames).arg(tableInfo.name));
	Q_ASSERT_X(success,"StoredSessionData::writeData","Error: "+query_->lastError().text().toLatin1());
	if(!success)
		return returnVal;

	while(query_->next())
	{
		QVariantList returnRow;
		for(int i=0;i<numColumns;i++)
			returnRow.append(query_->value(i));
		returnVal.append(returnRow);
	}
	success = query_->exec();
	return returnVal;
}

/*! \brief Generates a thread-specific connection to the Session database
 *
 *	The session database has threading issues because of the following:
 *		1. The session database may be accessed from different threads 
 *		   (remember, the commands coming into the server are processed
 *		   by different threads)
 *		2. Database connections can't be shared across threads
 *	The current solution to these problems is to create a unique database
 *	connection for each thread.  This is somewhat wasteful, but it's simple
 *	and it works.
 */
QSqlDatabase StoredSessionData::getSessionDb()
{
	Q_ASSERT(!sessionId_.isNull());
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
		sessionDb = QSqlDatabase::cloneDatabase(baseSessionDbConnection_,connectionName);
		sessionDb.open();
	}

	Q_ASSERT(sessionDb.isOpen());

	return sessionDb;
}