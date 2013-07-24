#ifndef _STORED_SESSION_DATA_H_
#define _STORED_SESSION_DATA_H_

#include <QVariantList>
#include <QMutex>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>
#include <QHash>

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
	void setTableInfo(int dataType,QString tableName,QString columnNames,QString columnTypes);

protected:
	bool startDataWrite(QString* error = NULL);
	//Should write the input data to descendant defined data structure
	void writeData(int dataType, QVariantList data);
	bool endDataWrite(QString* error = NULL);
	
	QList<int>readDataTypes();
	//Should read all data of the input dataType into a QVector or VariantList
	QList<QVariantList> readData(int dataType,QVariant condition,bool cut=false);

private:
	QSqlDatabase StoredSessionData::getSessionDb();
	struct TableInfo
	{
		TableInfo(){};
		TableInfo(QString n,QString cn,QString ct,QString ps){name = n;colNames = cn;colTypes = ct;placeholderString = ps;};
		QString name;
		QString colNames;
		QString colTypes;
		QString placeholderString;
	};
	QHash<int,TableInfo> tableByType_;
	QUuid sessionId_;
	QStringList openDatabaseConnections_;
	QSqlDatabase baseSessionDbConnection_;
	QSharedPointer<QSqlQuery> query_;
};

#endif

