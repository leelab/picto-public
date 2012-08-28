#include "ServerConfig.h"

#include <QCoreApplication>
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>
#include <QThread>
#include <QMutexLocker>
#include <QDir>

#include "../../common/memleakdetect.h"

QMutex ServerConfig::fileAccessMutex_;

ServerConfig::ServerConfig()
{
	//Create a unique connection name
	connectionName_ = QString("PictoServerConfigDatabase%1").arg((int)QThread::currentThreadId());

	//If we already have a connection open in this thread, use it, 
	//otherwise, create a new connection
	if(QSqlDatabase::contains(connectionName_))
	{
		configDb_ = QSqlDatabase::database(connectionName_);
	}
	else
	{
		configDb_ = QSqlDatabase::addDatabase("QSQLITE",connectionName_);
		QString configPath = QCoreApplication::applicationDirPath()+"/../config";
		QDir configDir(configPath);
		if(!configDir.exists())
		{
			configDir.mkpath(configPath);
			configDir = QDir(configPath);
		}
		configDb_.setDatabaseName(configDir.canonicalPath() + "/" + "PictoServer.config");
		configDb_.open();
		//Possibly create a new proxy table
		if(!configDb_.tables().contains("opensessions"))
		{
			QMutexLocker locker(&fileAccessMutex_);
			QSqlQuery query(configDb_);
			query.exec("CREATE TABLE opensessions (sessionID VARCHAR(38), "
													 "filepath VARCHAR(100), "
													 "lastactivity VARCHAR(20), "
													 "running INTEGER, "
													 "directorID VARCHAR(38), "
													 "proxyID VARCHAR(38))");
		}
	}

	Q_ASSERT(configDb_.isOpen());
}

ServerConfig::~ServerConfig()
{
	//configDb_.close();
}

//! \brief Inserts a session into the open sessions list
void ServerConfig::addSession(QString sessionID, QString filepath, QString directorID, QString proxyID)
{
	Q_ASSERT(configDb_.isOpen());
	QSqlQuery query(configDb_);

	query.prepare("INSERT INTO opensessions (sessionID,filepath,directorID,proxyID) "
		"VALUES (:sessionID,:filepath,:directorID,:proxyID)");
	query.bindValue(":sessionID", sessionID);
	query.bindValue(":filepath", filepath);
	query.bindValue(":directorID", directorID);
	query.bindValue(":proxyID", proxyID);
	QMutexLocker locker(&fileAccessMutex_);
	query.exec();
	locker.unlock();

	setActivity(sessionID,true);
}

//! \brief Removes a the session with the input ID from the database
void ServerConfig::removeSession(QString sessionID)
{
	Q_ASSERT(configDb_.isOpen());
	QSqlQuery query(configDb_);

	query.prepare("DELETE FROM opensessions WHERE "
		"sessionID=:sessionID");
	query.bindValue(":sessionID", sessionID);
	QMutexLocker locker(&fileAccessMutex_);
	query.exec();
}

//! \brief Returns the filepath of the database for the session with input ID
QString ServerConfig::getSessionPathByID(QString sessionID)
{
	Q_ASSERT(configDb_.isOpen());
	QSqlQuery query(configDb_);
	query.prepare("SELECT filepath FROM opensessions WHERE sessionID=:sessionID");
	query.bindValue(":sessionID", sessionID);
	QMutexLocker locker(&fileAccessMutex_);
	query.exec();

	QString path = "";
	if(query.next())
	{
		path = QString(query.value(0).toString());
	}
	return path;
}

//! \brief Returns the filepath of the database for the session associated with the input component
QString ServerConfig::getSessionPathByComponent(QString componentID)
{
	Q_ASSERT(configDb_.isOpen());
	QSqlQuery query(configDb_);
	query.prepare("SELECT filepath FROM opensessions WHERE ( directorID=:directorID ) OR ( proxyID=:proxyID )");
	query.bindValue(":directorID", componentID);
	query.bindValue(":proxyID", componentID);
	QMutexLocker locker(&fileAccessMutex_);
	query.exec();
	QString path = "";
	if(query.next())
	{
		path = query.value(0).toString();
	}
	return path;
}

//! \brief Returns the sessionID of the session associated with the input component
QString ServerConfig::getSessionIDByComponent(QString componentID)
{
	Q_ASSERT(configDb_.isOpen());
	QSqlQuery query(configDb_);
	query.prepare("SELECT sessionID FROM opensessions WHERE ( directorID=:directorID ) OR ( proxyID=:proxyID )");
	query.bindValue(":directorID", componentID);
	query.bindValue(":proxyID", componentID);
	QMutexLocker locker(&fileAccessMutex_);
	query.exec();
	QString sessionID = "";
	if(query.next())
	{
		sessionID = query.value(0).toString();
	}
	return sessionID;
}

//! \brief Sets the lastactivity field for the input session to the current time and changes the running field to the input value
void ServerConfig::setActivity(QString sessionID, bool running)
{
	if(sessionID == "")
		return;
	Q_ASSERT(configDb_.isOpen());
	QSqlQuery query(configDb_);

	QDateTime lastactivity = QDateTime::currentDateTime();
	query.prepare("UPDATE opensessions SET lastactivity=:lastactivity,running=:running WHERE sessionID=:sessionID");
	query.bindValue(":sessionID", sessionID);
	query.bindValue(":lastactivity", lastactivity.toString());
	query.bindValue(":running", int(running));
	QMutexLocker locker(&fileAccessMutex_);
	query.exec();
}

//! \brief Returns a list of all sessions with "running" field of true
QStringList ServerConfig::getRunningSessions()
{
	Q_ASSERT(configDb_.isOpen());
	QSqlQuery query(configDb_);
	QMutexLocker locker(&fileAccessMutex_);
	query.exec("SELECT sessionID FROM opensessions WHERE (running=1)");
	QStringList result;
	if(query.next())
	{
		result.append(query.value(0).toString());
	}
	return result;
}

//! \brief Returns a list of all sessions with "running" field of false and lastactivity time after input time
QStringList ServerConfig::getSessionsIdledBefore(QDateTime time)
{
	Q_ASSERT(configDb_.isOpen());
	QSqlQuery query(configDb_);
	QMutexLocker locker(&fileAccessMutex_);
	query.exec("SELECT lastactivity,sessionID FROM opensessions WHERE (running=0)");
	QStringList result;
	QDateTime lastActive;
	if(query.next())
	{
		lastActive.fromString(query.value(0).toString());
		if(lastActive < time)
			result.append(query.value(1).toString());
	}
	return result;
}