#include "ServerConfig.h"

#include <QCoreApplication>
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>
#include <QXmlStreamWriter>
#include <QThread>

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
		configDb_.setDatabaseName(QCoreApplication::applicationDirPath() + "/PictoServer.config");
		configDb_.open();
		//Possibly create a new proxy table
		if(!configDb_.tables().contains("proxyservers"))
		{
			QSqlQuery query(configDb_);
			query.exec("CREATE TABLE proxyservers (id INTEGER PRIMARY KEY, "
													 "name VARCHAR(30), "
													 "address VARCHAR(20), "
													 "port INT)");
		}
	}

	Q_ASSERT(configDb_.isOpen());
}

ServerConfig::~ServerConfig()
{
	//configDb_.close();
	int x=5;
	x++;
}

//! \brief Inserts a single proxy server into the configuration database
void ServerConfig::insertProxyServer(QString name, int port, QHostAddress address)
{
	Q_ASSERT(configDb_.isOpen());
	QSqlQuery query(configDb_);

	query.prepare("INSERT INTO proxyservers (name,address,port) "
		"VALUES (:name, :address,:port)");
	query.bindValue(":name", name);
	query.bindValue(":address", address.toString());
	query.bindValue(":port", port);
	query.exec();
}

//! \brief Removes a single prixy server fromt the configuration database
void ServerConfig::removeProxyServer(QString name, int port, QHostAddress address)
{
	Q_ASSERT(configDb_.isOpen());
	QSqlQuery query(configDb_);

	query.prepare("DELETE FROM proxyservers WHERE "
		"name=:name AND address=:address AND port=:port");
	query.bindValue(":name", name);
	query.bindValue(":address", address.toString());
	query.bindValue(":port", port);
	query.exec();
}

//! \brief Clears out the proxy server list.  This is used when the server starts up.
void ServerConfig::clearProxyServers()
{
	Q_ASSERT(configDb_.isOpen());
	QSqlQuery query(configDb_);
	query.exec("DELETE FROM proxyservers");
}

/*!	\brief Returns a list of director instances as an xml fragment.
 *
 *	The XML fragment will look like this:
 *	<Proxies>
 *		<Proxy>
 *			<Id>5</Id>
 *			<Name>Room 408 Proxy Server</Name>
 *		</Proxy>
 *		<Proxy>
 *			<Id>8</Id>
 *			<Name>Room 406 Proxy Server</Name>
 *		</Proxy>
 *	</Proxies> 
 */
QString ServerConfig::listProxyServers()
{
	Q_ASSERT(configDb_.isOpen());
	QString xmlFragment;
	QXmlStreamWriter xmlWriter(&xmlFragment);

	xmlWriter.writeStartElement("Proxies");

	QSqlQuery query(configDb_);
	Q_ASSERT(query.exec("SELECT id, name FROM proxyservers"));

	while(query.next())
	{
		xmlWriter.writeStartElement("Proxy");
		xmlWriter.writeTextElement("Id",QString::number(query.value(0).toInt()));
		xmlWriter.writeTextElement("Name",query.value(1).toString());
		xmlWriter.writeEndElement(); //Proxy
	}
	xmlWriter.writeEndElement();	//Proxies

	return xmlFragment;
}

//! Returns true if the passed in ID refers to a running proxy server
bool ServerConfig::proxyIdIsValid(int id)
{
	//-1 indicates that we aren't going to be recording neural data
	if(id == -1)
		return true;

	Q_ASSERT(configDb_.isOpen());
	QSqlQuery query(configDb_);
	query.prepare("SELECT id FROM proxyservers WHERE id = :id");
	query.bindValue(":id", id);
	query.exec();

	if(query.next())
		return true;
	else
		return false;
}

//! Return the address of a proxy server given its id
QHostAddress ServerConfig::proxyServerAddress(int id)
{
	Q_ASSERT(configDb_.isOpen());
	QHostAddress addr;
	if(!proxyIdIsValid(id))
		return addr;

	QSqlQuery query(configDb_);
	query.prepare("SELECT address FROM proxyservers WHERE id = :id");
	query.bindValue(":id", id);
	query.exec();

	if(query.next())
		addr = QHostAddress(query.value(0).toString());

	return addr;
}

//! Return the port of a proxy server given its id
int ServerConfig::proxyServerPort(int id)
{
	Q_ASSERT(configDb_.isOpen());
	int port = -1;
	if(!proxyIdIsValid(id))
		return port;

	QSqlQuery query(configDb_);
	query.prepare("SELECT port FROM proxyservers WHERE id = :id");
	query.bindValue(":id", id);
	query.exec();

	if(query.next())
		port = query.value(0).toInt();

	return port;
}
