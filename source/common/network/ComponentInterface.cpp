#include <QApplication>
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>
#include <QDir>

#include "ComponentInterface.h"
#include "../common/network/ServerDiscoverer.h"

#include "../common/protocol/EndSessionResponseHandler.h"
#include "../common/protocol/ErrorResponseHandler.h"
#include "../common/protocol/LoadExpResponseHandler.h"
#include "../common/protocol/NewSessionResponseHandler.h"
#include "../common/protocol/OKResponseHandler.h"
#include "../common/protocol/PauseResponseHandler.h"
#include "../common/protocol/ResumeResponseHandler.h"
#include "../common/protocol/RewardResponseHandler.h"
#include "../common/protocol/StartResponseHandler.h"
#include "../common/protocol/StopResponseHandler.h"
#include "../memleakdetect.h"
using namespace Picto;
ComponentInterface::ComponentInterface(QString type)
:componentType_(type)
{
	deviceOpened_ = false;
	QString dbName = "Picto" + type + "Interface";
	dbName = dbName.toLower();
	configDb_ = QSqlDatabase::addDatabase("QSQLITE",dbName);
	QString configPath = QCoreApplication::applicationDirPath()+"/../config";
	QDir configDir(configPath);
	if(!configDir.exists())
	{
		configDir.mkpath(configPath);
		configDir = QDir(configPath);
	}
	configDb_.setDatabaseName(configDir.canonicalPath() + "/" + dbName + ".config");
	configDb_.open();

	QSqlQuery query(configDb_);
	if(!configDb_.tables().contains("componentinfo"))
	{
		query.exec("CREATE TABLE componentinfo (key TEXT, value TEXT)");
		componentId_ = QUuid::createUuid();
		query.prepare("INSERT INTO componentinfo (key,value) VALUES ('id',:id)");
		query.bindValue(":id",QString(componentId_));
		query.exec();
	}
	query.exec("SELECT value FROM componentinfo WHERE key='id'");
	bool rc = query.next();
	Q_ASSERT(rc);
	componentId_ = QUuid(query.value(0).toString());
}

ComponentInterface::~ComponentInterface()
{
	configDb_.close();
}

QString ComponentInterface::componentType()
{
	return componentType_;
}
int ComponentInterface::activate()
{
	//while(dataCommandChannel_.isNull() || !dataCommandChannel_->assureConnection() )
	//	connectToServer(dataCommandChannel_,componentID, componentType());
	if(dataCommandChannel_.isNull())
		dataCommandChannel_ = QSharedPointer<Picto::CommandChannel>(new Picto::CommandChannel(componentId_,componentType()));
	openDevice();
	deviceOpened_ = true;
	Q_ASSERT(!statusManager_.isNull());
	dataCommandChannel_->setStatusManager(statusManager_);
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new NewSessionResponseHandler(statusManager_,dataCommandChannel_)),false);
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new EndSessionResponseHandler(statusManager_,dataCommandChannel_)),false);
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new ErrorResponseHandler(statusManager_)),false);
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new LoadExpResponseHandler(statusManager_)),false);
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new OKResponseHandler(statusManager_)),false);
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new PauseResponseHandler(statusManager_)),false);
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new ResumeResponseHandler(statusManager_)),false);
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new RewardResponseHandler(statusManager_)),false);
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new StartResponseHandler(statusManager_)),false);
	dataCommandChannel_->addResponseHandler(QSharedPointer<ProtocolResponseHandler>(new StopResponseHandler(statusManager_)),false);

	dataCommandChannel_->processResponses(-1);

	return 0;
}
int ComponentInterface::deActivate()
{
	if(deviceOpened_)
		closeDevice();
	statusManager_->setStatus(idle);
	return 0;
}