#ifndef COMPONENTINTERFACE_H
#define COMPONENTINTERFACE_H

#include <QTimer>
#include <QUuid>
#include <QSqlDatabase>
#include "CommandChannel.h"
#include "ComponentStatusManager.h"

/*! \brief A base class for high level components that take orders from the Picto server and provide it with session data.
 *	\details Currently, the Picto Director and Proxy are the two ComponentInterface users types.  Originally these two functioned very
 *	differently, with the Director pushing data to the Server and the server polling the Proxy for data whenever it was ready for it.
 *	This seemed silly since the Proxy is always gathering at least lfp data and certainly wants to offload it to the server, whereas the server pretty much just always
 *	wants whatever data is available and never needs to poll the same data twice, so we decided to create one system that could be reused in both cases, and the 
 *	ComponentInterface is that system.  It is a fairly simple class.  ComponentInterfaces can be activated and deactivated and they should include functionality for opening 
 *	and closing underlying devices in implementations of openDevice() and closeDevice().
 *	ComponentInterfaces always include an underlying CommandChannel which sends data to the Picto Server, they have a unique ID to identify them and
 *	include a type name to identify the role of their data in an overall session.  They also include a ComponentStatusManager which
 *	tracks the status of the session and handles periodic updates to the Server informing it of the current ComponentInterface status
 *	and generally just keeping the connection alive.
 *	\note As discussed at length in the CommandChannel documentation (particularly CommandChannel::processResponses()), ComponentInterface design suffers considerably from the design restriction
 *	of not using the Qt Event Loop.  That restriction was imposed when we used to think the Qt Event Loop would slow things down to an extent that we might miss stimulus frames
 *	during expeiriment runs in the Picto Director.  We should make it a priority to remove this restriction now that
 *	computer speeds are not as restrictive as they were when that decision was made.  We should refactor this class, along with the entire experimental run paradigm 
 *	to work within the Qt Event Loop.
 *	\sa ComponentStatusManager, CommandChannel, CommandChannel::processResponses()
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API ComponentInterface
#else
class ComponentInterface : public QObject
#endif
{
public:
	ComponentInterface(QString type);
	virtual ~ComponentInterface();
	QString componentType();
	virtual int activate();
	virtual int deActivate();
protected:
	/*! \brief Returns the name of this COmponentInterface.
	*/
	virtual QString name() = 0;

	//! \brief Initialize any devices and objects used by the component
	virtual int openDevice(){return 0;}
	//! \brief Deinitialize any devices and objects used by the component
	virtual int closeDevice(){return 0;}
	//! \brief The command channel used to send data to the server and recieve responses.
	QSharedPointer<Picto::CommandChannel> dataCommandChannel_;
	//! \brief Tracks the status of the session, handles periodic updates to the Server and generally just keeps the ComponentInterface - Server connection alive.
	QSharedPointer<ComponentStatusManager> statusManager_;
	//! \brief The unique ID of this ComponentInterface
	QUuid componentId_;
	//! \brief A string describing the type of this ComponentInterface
	QString componentType_;
private:
	ComponentStatus status_;
	QSqlDatabase configDb_;
	bool deviceOpened_;
};
#endif