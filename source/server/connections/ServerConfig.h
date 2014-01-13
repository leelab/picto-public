#ifndef _SERVER_CONGIF_H_
#define _SERVER_CONGIF_H_

#include <QSqlDatabase>
#include <QString>
#include <QDateTime>
#include <QMutex>

/*! \brief ServerConfig handles the configuration of a PictoServer
 *
 *	\details The configuration of the server is stored in a config database.  This
 *	object provides an interface for accessing the database, since we don't
 *	want to have access by random code.
 *
 *	Currently, the Server configuration consists exclusively of information about the currently open
 *	and recently timed out sessions, the components attached to them and their associated Sqlite files.
 *	Functions are provided to access and change these values.  In the future, other Server configurations
 *	may be needed and can be added here.
 *
 *	\note A new (and unique) database connection is created everytime a ServerConfig
 *	object is constructed.  This is done because database connections can only be used
 *	from within the thread that created them, so we can't just create a single connection.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class ServerConfig
{
public:
	ServerConfig();
	virtual ~ServerConfig();

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