#ifndef COMPONENTINTERFACE_H
#define COMPONENTINTERFACE_H

#include <QTimer>
#include <QUuid>
#include "CommandChannel.h"

#if defined WIN32 || defined WINCE
class PICTOLIB_API ComponentInterface
#else
class ComponentInterface
#endif
{
public:
	ComponentInterface();
protected:
	virtual int activate();
	virtual int deActivate();
	static QSharedPointer<Picto::CommandChannel> connectToServer(QUuid componentID, QString componentType);
	virtual QString componentType() = 0;
	virtual QString name() = 0;
	//! \brief Extend this to initialize any devices and objects used by the component
	virtual int openDevice() = 0;
	//! \brief Extend this to deinitialize any devices and objects used by the component
	virtual int closeDevice() = 0;
	//! \brief Extend this to perform any actions necessary when NEWSESSION is called by PictoServer. 
	virtual int startSession(QUuid sessionID) = 0;
	//! \brief Extend this to perform any actions necessary when LOADEXP is called by PictoServer.
	virtual int loadExp() = 0;
	//! \brief Extend this to perform any actions necessary when START is called by PictoServer.
	virtual int startExp(QString expName) = 0;
	//! \brief Extend this to perform any actions necessary to stop and exit a running experiment.
	virtual int stopExp() = 0;
	//! \brief Extend this to perform any actions necessary when REWARD is called by PictoServer.
	virtual int reward(int channel) = 0;
	//! \brief Extend this to perform any actions/notifications necessary when ERROR is called by PictoServer.
	virtual int reportError() = 0;
	//! \brief Extend this to perform any actions/notifications necessary when an unsupported command is called by PictoServer.
	virtual int reportUnsupported() = 0;
	//! \brief Extend this to perform any actions necessary to end a running session.
	virtual int endSession() = 0;
	QSharedPointer<Picto::CommandChannel> serverUpdateChannel_;	// This is the command channel used to update the server as to the component status.
	QSharedPointer<Picto::CommandChannel> dataCommandChannel_;	// This is the command channel used to send data to the server.
private:
	bool active_;
	bool deviceOpened_;
	bool sessionStarted_;
	bool expStarted_;
};
#endif