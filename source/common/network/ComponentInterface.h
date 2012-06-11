#ifndef COMPONENTINTERFACE_H
#define COMPONENTINTERFACE_H

#include <QTimer>
#include <QUuid>
#include <QSqlDatabase>
#include "CommandChannel.h"
#include "ComponentStatusManager.h"

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
	virtual QString name() = 0;

	//! \brief Overload this to initialize any devices and objects used by the component
	virtual int openDevice(){return 0;}
	//! \brief Overload this to deinitialize any devices and objects used by the component
	virtual int closeDevice(){return 0;}
	QSharedPointer<Picto::CommandChannel> dataCommandChannel_;	// This is the command channel used to send data to the server.
	QSharedPointer<ComponentStatusManager> statusManager_;
	QUuid componentId_;
	QString componentType_;
private:
	ComponentStatus status_;
	QSqlDatabase configDb_;
	bool deviceOpened_;
};
#endif