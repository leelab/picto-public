#ifndef _REMOTESTATEUPDATER_H_
#define _REMOTESTATEUPDATER_H_
#include <QObject>
#include <QString>
#include <QStringList>

#include "../../common/network/CommandChannel.h"
#include "../../common/engine/stateupdater.h"

namespace Picto {

class RemoteStateXMLHandler;

/*! \brief A StateUpdater that gathers Session events from the Picto Server in real time.
 *
 *	\details This StateUpdater is used in the Remote Viewer to keep the local slave Experiment in sync with the
 *	master Experiment running on the Director.  It is used in the RemoteViewer's SlaveExperimentDriver which
 *	doesn't care about the StateUpdater implementation.  The result is that we can use the same system that we
 *	use for Experimental playaback for viewing live Experiments.  The only difference is that we don't currently
 *	support Analysis of live Experiments.  This is partially due to the fact that no future data is available
 *	at any given time.  It is for this reason that we are returning empty pointers from all of the StateUpdater
 *	get?Reader() functions.
 *	\author Trevor, Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class RemoteStateUpdater : public StateUpdater
{
	Q_OBJECT
public:
	RemoteStateUpdater(CommandChannel *serverChan);
	void initForNewSession();
	virtual bool updateState();
	/*! \brief Returns an empty pointer since Analaysis Data Readers aren't yet supported in live Experiments.*/
	virtual QSharedPointer<FrameReader> getFrameReader(){return QSharedPointer<FrameReader>();};
	/*! \brief Returns an empty pointer since Analaysis Data Readers aren't yet supported in live Experiments.*/
	virtual QSharedPointer<LfpReader> getLfpReader(){return QSharedPointer<LfpReader>();};
	/*! \brief Returns an empty pointer since Analaysis Data Readers aren't yet supported in live Experiments.*/
	virtual QSharedPointer<RewardReader> getRewardReader(){return QSharedPointer<RewardReader>();};
	/*! \brief Returns an empty pointer since Analaysis Data Readers aren't yet supported in live Experiments.*/
	virtual QSharedPointer<RunNotesReader> getRunNotesReader(){return QSharedPointer<RunNotesReader>();};
	/*! \brief Returns an empty pointer since Analaysis Data Readers aren't yet supported in live Experiments.*/
	virtual QSharedPointer<SpikeReader> getSpikeReader(){return QSharedPointer<SpikeReader>();};
	/*! \brief Returns an empty list since Analaysis Data Readers aren't yet supported in live Experiments.*/
	virtual QStringList getSignalReaderNames(){return QStringList();};
	/*! \brief Returns an empty pointer since Analaysis Data Readers aren't yet supported in live Experiments.*/
	virtual QSharedPointer<SignalReader> getSignalReader(QString name){return QSharedPointer<SignalReader>();};

signals:
	/*! \brief Never called.
	 *	\note The startingRun and endingRun signals are not yet being called by the RemoteStateUpdater
	 *	this is because its current design doesn't allow it to differentiate between a run that is starting
	 *	and a run that it is joining in the middle.  Similarly for an ending run, the current design doesn't
	 *	know if the run ended or if the connection was lost or some data was missed due to a slow connection.
	 */
	void startingRun(QString taskName,QString runName);
	/*! \brief Never called.
	 *	\note The startingRun and endingRun signals are not yet being called by the RemoteStateUpdater
	 *	this is because its current design doesn't allow it to differentiate between a run that is starting
	 *	and a run that it is joining in the middle.  Similarly for an ending run, the current design doesn't
	 *	know if the run ended or if the connection was lost or some data was missed due to a slow connection.
	 */
	void endingRun();

	/*!\brief Emitted when a Property value changes for a Property with an AssetId of propId and a new value of value.*/
	void propertyValueChanged(int propId, QString value);
	/*!\brief Emitted when a Property initValue changes for a Property with an AssetId of propId and a new initValue of value.*/
	void propertyInitValueChanged(int propId, QString value);
	/*! \brief Emitted when a Transition with AssetId of transId is traversed.*/
	void transitionActivated(int transId);
	/*! \brief Emitted when a new frame is presented.  time is the time at which the first phosphor appeared on the display for this frame.*/
	void framePresented(double time);
	/*! \brief Emitted when a reward is triggered.  
	 *	\details time is the time when the reward starts, duration is its duration in ms, channel is the 
	 *	reward channel on which the reward was provided.
	 */
	void rewardSupplied(double time,int duration,int channel);
	/*! \brief Emitted when new signal data comes in.
	 *	\details name is the name of the SignalChannel for which new data is available.  subChanNames is a list of names of this SIgnalChannel's
	 *	sub-channels.  vals is a list of the new values.  If subChanNames is x,y, vals will have data ordered like x1,y1,x2,y2,x3,y3,...
	 *	\note Unlike in the master, signal data only comes in once per frame, not once per SignalChannel sample period.  signalChanged() should
	 *	be emitted with all of the SignalChannel data since the previous frame after all Property value change and transition traversal signals
	 *	have been emitted and before framePresented() is emitted.
	 */
	void signalChanged(QString name,QStringList subChanNames,QVector<float> vals);
public slots:
	/*! \brief Updates the lastTimeStateDataRequested_ if it's passed a more recent time.
	 */
	void updateCurrUnitTime(QString time);
private:
	CommandChannel *serverChan_;
	QString lastTimeStateDataRequested_;
	QMap<QString, QSharedPointer<RemoteStateXMLHandler>> xmlHandlers_;
};


}; //namespace Picto

#endif
