#ifndef _STORED_SESSION_DATA_H_
#define _STORED_SESSION_DATA_H_

#include <QVariantList>
#include <QMutex>
#include <QSharedPointer>
#include <QUuid>
#include <QHash>
#include <QSqlQuery>

#include "SessionData.h"

/*!	\brief Interface to an object that stores session data
 *
 */

class StoredSessionData : public SessionData
{
public:
	StoredSessionData::StoredSessionData(QString sessionPath,QString databaseFileName);
	virtual ~StoredSessionData();
	void setSessionId(QUuid sessionId);
	//Note that indeces are not built when you call this function.  You must use buildTableIndeces to do that later.
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

