#include <QApplication>
#include "ComponentInterface.h"
#include "../common/network/ServerDiscoverer.h"
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>

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
using namespace Picto;
ComponentInterface::ComponentInterface()
{
	deviceOpened_ = false;
}
int ComponentInterface::activate()
{
	QString dbName = "Picto" + componentType() + "Interface";
	dbName = dbName.toLower();
	configDb_ = QSqlDatabase::addDatabase("QSQLITE",dbName);
	configDb_.setDatabaseName(QCoreApplication::applicationDirPath() + "/" + dbName + ".config");
	configDb_.open();

	QUuid componentID;
	QSqlQuery query(configDb_);
	if(!configDb_.tables().contains("componentinfo"))
	{
		query.exec("CREATE TABLE componentinfo (key TEXT, value TEXT)");
		componentID = QUuid::createUuid();
		query.prepare("INSERT INTO componentinfo (key,value) VALUES ('id',:id)");
		query.bindValue(":id",QString(componentID));
		query.exec();
	}
	query.exec("SELECT value FROM componentinfo WHERE key='id'");
	Q_ASSERT(query.next());
	componentID = QUuid(query.value(0).toString());

	//while(dataCommandChannel_.isNull() || !dataCommandChannel_->assureConnection() )
	//	connectToServer(dataCommandChannel_,componentID, componentType());
	if(dataCommandChannel_.isNull())
		dataCommandChannel_ = QSharedPointer<Picto::CommandChannel>(new Picto::CommandChannel(componentID,componentType()));
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