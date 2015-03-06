#ifndef _STORED_SESSION_DATA_H_
#define _STORED_SESSION_DATA_H_

#include <QVariantList>
#include <QMutex>
#include <QSharedPointer>
#include <QUuid>
#include <QHash>
#include <QSqlQuery>

#include "SessionData.h"

/*!	\brief Handles long term storage of session data in an SQL file.
 *	\details This is meant to be used with CachedSessionData objects.  Those objects should
 *	periodically move their data to a StoredSessionData object using moveDataTo().  The
 *	StoredSessionData object writes the data to an SQL file according to the table structure
 *	entered in the setTableInfo() function.  This object also supports setting up of SQL indeces
 *	by using the setTableInfo() function to define index columns and calling buildTableIndeces()
 *	to actually create them (typically when a Session is ending).  Individual values can
 *	be entered into the object by using insertData().
 *	\note This object groups batched writeData() operations into a single SQL transaction.  See
 *	startDataWrite() for more details.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class StoredSessionData : public SessionData
{
public:
	StoredSessionData::StoredSessionData(QString sessionPath,QString databaseFileName);
	virtual ~StoredSessionData();
	void setSessionId(QUuid sessionId);
	void setTableInfo(int dataType,QString tableName,QString columnNames,QString columnTypes,QString indexedColumns = "");
	void buildTableIndeces();
	void insertData(int dataType, QVariantList data);

protected:
	bool startDataWrite(QString* error = NULL);
	//Should write the input data to descendant defined data structure
	void writeData(int dataType, QVariantList data);
	bool endDataWrite(QString* error = NULL);
	
	QList<int>readDataTypes();
	//Should read all data of the input dataType into a QVector or VariantList
	QList<QVariantList> readData(int dataType,QVariant condition,bool cut=false);
	virtual void eraseEverything();

private:
	QSqlDatabase StoredSessionData::getSessionDb();
	struct TableInfo
	{
		TableInfo(){};
		TableInfo(QString n,QString cn,QString ct,QString ic,QString ps){name = n;colNames = cn;colTypes = ct;indexedColumns = ic;placeholderString = ps;};
		QString name;
		QString colNames;
		QString colTypes;
		QString indexedColumns;
		QString placeholderString;
	};
	QHash<int,TableInfo> tableByType_;
	QUuid sessionId_;
	QStringList openDatabaseConnections_;
	QString baseSessionDbConnection_;
	QSharedPointer<QSqlQuery> query_;
};

#endif

