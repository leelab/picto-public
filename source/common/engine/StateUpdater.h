#ifndef _STATEUPDATER_H_
#define _STATEUPDATER_H_
#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>

#include "../common.h"
#include "../playback/PlaybackInterfaces.h"

namespace Picto {

/*! StateUpdater
 *
 *	Parent class for objects that update the Picto State
 *	from some external source
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API StateUpdater : public QObject
#else
class StateUpdater : public QObject
#endif
{
	Q_OBJECT
public:
	StateUpdater(){};
	virtual ~StateUpdater(){};
	//Updates the picto state to the latest values
	virtual bool updateState() = 0;

	virtual QSharedPointer<FrameReader> getFrameReader() = 0;
	virtual QSharedPointer<RewardReader> getRewardReader() = 0;
	virtual QSharedPointer<SpikeReader> getSpikeReader() = 0;
	virtual QStringList getSignalReaderNames() = 0;
	virtual QSharedPointer<SignalReader> getSignalReader(QString name) = 0;
signals:
	void startingRun(QString taskName,QString runName);
	void endingRun();
	//triggered when values change
	void propertyValueChanged(int propId, QString value);
	void propertyInitValueChanged(int propId, QString value);
	void transitionActivated(int transId);
	void framePresented(double time);
	void rewardSupplied(double time,int duration,int channel);
	void signalChanged(QString name,QStringList subChanNames,QVector<float> vals);
	void loadedTo(double maxBehavioral,double maxNeural);
	void disableRendering(bool disable);
};


}; //namespace Picto

#endif
