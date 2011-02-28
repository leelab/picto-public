#ifndef COMPONENTINTERFACE_H
#define COMPONENTINTERFACE_H

#include <QTimer>
#include <QUuid>
#include <QSqlDatabase>
#include "CommandChannel.h"
enum ComponentStatus
{
	idle, stopped, paused, running
};

#if defined WIN32 || defined WINCE
class PICTOLIB_API ComponentInterface
#else
class ComponentInterface
#endif
{
public:
	ComponentInterface();
	virtual int activate();
	virtual int deActivate();
protected:
	static QSharedPointer<Picto::CommandChannel> connectToServer(QUuid componentID, QString componentType);
	void setStatus(ComponentStatus status){status_ = status;}
	ComponentStatus getStatus(){return status_;};
	QString getStatusString();
	virtual QString componentType() = 0;
	virtual QString name() = 0;
	/*!	\brief Overload this to return false if the interface should stop running and exit.
	 *	If action isn't busy in code from a child class, this will be called at least once per COMPONENTUPDATE message.
	 */
	virtual bool continueRunning(){return true;};
	//! \brief Overload this to initialize any devices and objects used by the component
	virtual int openDevice(){return 0;}
	//! \brief Overload this to deinitialize any devices and objects used by the component
	virtual int closeDevice(){return 0;}
	//! \brief Overload this to perform any actions necessary when NEWSESSION is called by PictoServer. 
	virtual int startSession(QUuid sessionID){sessionID = QUuid();return 0;}
	//! \brief Overload this to perform any actions necessary when LOADEXP is called by PictoServer.
	virtual int loadExp(QString expXML){expXML = "";return 0;}
	//! \brief Overload this to perform any actions necessary when START is called by PictoServer.
	virtual int startExp(QString taskName){taskName = "";return 0;}
	//! \brief Overload this to perform any actions necessary to stop and exit a running experiment.
	virtual int stopExp(){return 0;}
	//! \brief Overload this to perform any actions necessary when REWARD is called by PictoServer.
	virtual int reward(int channel){channel = 0;return 0;}
	//! \brief Called when the server connects or disconnects with appropriate input.
	virtual int reportServerConnectivity(bool connected){connected = false;return 0;}
	//! \brief Overload this to perform any actions/notifications necessary when ERROR is called by PictoServer.
	virtual int reportErrorDirective(){return 0;}
	//! \brief Overload this to perform any actions/notifications necessary when an unsupported command is called by PictoServer.
	virtual int reportUnsupportedDirective(QString directive){directive = "";return 0;}
	//! \brief Overload this to perform any actions necessary to end a running session.
	virtual int endSession(){return 0;}
	QSharedPointer<Picto::CommandChannel> serverUpdateChannel_;	// This is the command channel used to update the server as to the component status.
	QSharedPointer<Picto::CommandChannel> dataCommandChannel_;	// This is the command channel used to send data to the server.
private:
	void ServerConnectivityUpdate(bool connected);
	ComponentStatus status_;
	QSqlDatabase configDb_;
	bool active_;
	bool deviceOpened_;
	bool sessionStarted_;
	bool expStarted_;
	bool serverWasConnected_;
};
#endif