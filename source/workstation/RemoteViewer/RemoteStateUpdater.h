#ifndef _REMOTESTATEUPDATER_H_
#define _REMOTESTATEUPDATER_H_
#include <QObject>
#include <QString>
#include <QStringList>

#include "../../common/network/CommandChannel.h"
#include "../../common/engine/stateupdater.h"

namespace Picto {

/*! RemoteStateUpdater
 *
 *	Extends StateUpdater to update the Picto State based on
 *	the latest data on the Picto Server
 */

class RemoteStateUpdater : public StateUpdater
{
	Q_OBJECT
public:
	RemoteStateUpdater(CommandChannel *serverChan);
	//Updates the picto state to the latest values
	virtual bool updateState();
signals:
	//triggered when values change
	void propertyChanged(int propId, QString value);
	void transitionActivated(int transId);
	void framePresented(double time);
	void rewardSupplied(double time,int duration,int channel);
	void signalChanged(QString name,QStringList subChanNames,QVector<float> vals);

private:
	CommandChannel *serverChan_;
	QString lastTimeStateDataRequested_;
};


}; //namespace Picto

#endif
