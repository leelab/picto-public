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
	void initForNewSession();
	//StateUpdater Interface
	//Updates the picto state to the latest values
	virtual bool updateState();
	virtual QSharedPointer<FrameReader> getFrameReader(){return QSharedPointer<FrameReader>();};
	virtual QSharedPointer<RewardReader> getRewardReader(){return QSharedPointer<RewardReader>();};
	virtual QSharedPointer<SpikeReader> getSpikeReader(){return QSharedPointer<SpikeReader>();};
	virtual QStringList getSignalReaderNames(){return QStringList();};
	virtual QSharedPointer<SignalReader> getSignalReader(QString name){return QSharedPointer<SignalReader>();};

signals:
	//NOTE: The startingRun and endingRun signals are not yet being called by the RemoteStateUpdater
	//this is because its current design doesn't allow it to differentiate between a run that is starting
	//and a run that it is joining in the middle.  Similarly for an ending run, the current design doesn't
	//know if the run ended or if the connection was lost or some data was missed due to a slow connection.
	void startingRun(QString taskName,QString runName);
	void endingRun();

	//triggered when values change
	void propertyValueChanged(int propId, QString value);
	void propertyInitValueChanged(int propId, QString value);
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
