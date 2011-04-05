#ifndef _SERVER_CONGIF_H_
#define _SERVER_CONGIF_H_

#include <QSqlDatabase>
#include <QString>
#include <QDateTime>
#include <QMutex>

/*! \brief ServerConfig handles the configuration of a PictoServer
 *
 *	The configuration of the server is stored in a config database.  This
 *	object provides an interface for accessing the database, since we don't
 *	want to have access by random code.
 *
 *	NOTE: A new (and unique) database connection is created everytime a ServerConfig
 *	object is constructed.  This is done because database connections can only be used
 *	from within the thread that created them, so we can't just create a single connection.
 *
 *	The rather odd use of a QSqlDatabase pointer needs some explanation.  You can't remove a
 *	database connection if there is still a database that used that connection.  This is true, 
 *	even if you call close on the database object.  Because I wanted to 
 */

class ServerConfig
{
public:
	ServerConfig();
	~ServerConfig();

	void addSession(QString sessionID, QString filepath, QString directorID, QString proxyID);
	void removeSession(QString sessionID);
	QString getSessionPathByID(QString sessionID);
	QString getSessionPathByComponent(QString componentID);
	QString getSessionIDByComponent(QString componentID);
	void setActivity(QString sessionID, bool running);
	QStringList getRunningSessions();
	QStringList getSessionsIdledBefore(QDateTime time);

private:
	QSqlDatabase configDb_;
	QString connectionName_;
	static QMutex fileAccessMutex_;
};

#endif