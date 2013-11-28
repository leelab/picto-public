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

/*! \brief Creates a new ComponentInterface with the input type name (ie. "Director")
 *	\details This constructor also automatically creates a config file in the config directory
 *	according to the type string and stores this ComponentInterface objects ComponentID (a unique
 *	ID identifying this component device) inside.
 *	
*/
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
		query.bindValue(":id",componentId_.toString());
		query.exec();
	}
	query.exec("SELECT value FROM componentinfo WHERE key='id'");
	bool rc = query.next();
	Q_ASSERT(rc);
	componentId_ = QUuid(query.value(0).toString());
}

/*! \brief Closes this ComponentInterface object's config database before this object is destroyed.
*/
ComponentInterface::~ComponentInterface()
{
	configDb_.close();
}
/*! \brief Returns a string describing this type of ComponentInterface (it was passed into the constructor).
*/
QString ComponentInterface::componentType()
{
	return componentType_;
}
/*! \brief Called to activate this component and run this component.
 *	\details Calls openDevice() to initialize and devices and objects used by this component interface.
 *	Adds default ProtocolResponseHandler objects to this object's data CommandChannel to handle responding to standard ProtocolResponse types that
 *	may come in from the server.
 *	Runs the ComponentInterface by calling CommandChannel::processResponses() with input of -1.  This is essentially the application's event loop.  It causes the 
 *	CommandChannel to loop endlessly listening for commands within Server responses, handle those responses, and periodically call update on the statusManager_.  The
 *	function only returns when the escape key is pressed.  See CommandChannel::processResponses() for more detail.
 *	\sa CommandChannel::processResponses()
 */
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
/*! \brief Deactivates the component interface to prepare it for closing.
 *	\details Calls closeDevice() to deinitialize any devices and objects used by the component and sets the ComponentStatusManager status to idle.
 */
int ComponentInterface::deActivate()
{
	if(deviceOpened_)
		closeDevice();
	statusManager_->setStatus(idle);
	return 0;
}