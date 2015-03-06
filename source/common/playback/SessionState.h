#ifndef _SESSIONSTATE_H_
#define _SESSIONSTATE_H_
#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include <QVector>
#include <QSqlDatabase>
#include "FrameState.h"
#include "LfpState.h"
#include "PropertyState.h"
#include "RewardState.h"
#include "SignalState.h"
#include "SpikeState.h"
#include "RunNotesState.h"
#include "TransitionState.h"
#include "PlaybackInterfaces.h"
#include "../common.h"

namespace Picto {

/*! \brief This is pretty much a convenience class for grouping together DataState objects for
 *	playback and Analysis purposes.
 *	\details When the SessionState is created, it creates all of the standard DataState objects
 *	for playback.  addSignal() can be used to create and add SignalState DataState objects since the number of
 *	Signal channels in a session is variable as a function of the Director hardware and configuration.  setSessionData()
 *	loads session data into all of the DataState objects, startNewRun() sets the active run, and various get functions
 *	can then be used to access the DataState objects and traverse through them for playback purposes by using their
 *	moveToIndex() functions.  Value changes due to the playback process are emitted as signals from this SessionState.
 *	For Analysis purposes, various get functions are available to retrieve Reader objects which can be used to poll
 *	data over set time intervals.  There is also the capability to disable Lfp data for lower RAM usage and load time.
 *	
 *	\note Some of the functionality that currently appears in the SessionPlayer might better appear here.  Ideally, this
 *	class would encapsulate all of the DataState objects to the extent that they wouldn't need to be accessed from
 *	the outside world.  This would be a good thing to work on when someone has some free time.
 *	\sa SessionLoader, SessionPlayer
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SessionState : public QObject
#else
class SessionState : public  QObject
#endif
{
	Q_OBJECT
public:
	SessionState(bool enableLfp = true);
	virtual ~SessionState();
	//SET FUNCTIONS-----------------------------------------------------------------
	void setSessionData(QSqlDatabase session,QHash<int,bool> obsoleteAssetIds = QHash<int,bool>());
	void startNewRun(double startTime,double endTime=-1);
	void restartRun();
	void addSignal(QString name,QString tableName,QStringList subChanNames,double sampPeriod);
	void enableLfp(bool enable);
	bool lfpEnabled();

	//GET FUNCTIONS-----------------------------------------------------------------
	//The separate types of DataReaders are used to access asynchronous data
	//such as spikes since 1 second ago, lfp data until 500ms from now, etc.
	QSharedPointer<FrameReader> getFrameReader();
	QSharedPointer<RewardReader> getRewardReader();
	QSharedPointer<RunNotesReader> getRunNotesReader();
	QStringList getSignalReaderNames();
	QSharedPointer<SignalReader> getSignalReader(QString name);
	QSharedPointer<LfpReader> getLfpReader();
	QSharedPointer<SpikeReader> getSpikeReader();

	//The separate types of DataStates are used to step through the session.
	QSharedPointer<PropertyState> getPropertyState();
	QSharedPointer<TransitionState> getTransitionState();
	QSharedPointer<FrameState> getFrameState();
	QSharedPointer<RewardState> getRewardState();
	QSharedPointer<SignalState> getSignalState(QString name);
	QSharedPointer<LfpState> getLfpState();
	QSharedPointer<SpikeState> getSpikeState();

	QList<QSharedPointer<DataState>> getStatesIndexedById();
	QList<QSharedPointer<DataState>> getStatesIndexedByTime();
signals:
	
	//Synchronous Data Signals, triggered when values change

	/*! \brief Emitted whenever a Property value change is traversed as part of playback.
	 *	\details The propId is the Asset Id of the Property, the value is the new value of the Property.
	 */
	void propertyChanged(int propId, QString value);
	/*! \brief Emitted whenever a Property initValue change is traversed as part of playback.
	 *	\details The propId is the Asset Id of the Property, the value is the new value of the Property.
	 *	InitValues are the values to which Property values are set when their parents first enter scope.
	 *	When the Operator changes values during playback, these are the values that are changed.  They then
	 *	take effect and affect the run values when their parents next enter scope.
	 */
	void propertyInitValueChanged(int propId, QString value);
	/*! \brief Emitted whenever a Transition traversal is passed over as part of playback.
	 *	\details The transId is the Asset Id of the Transition that was traversed.
	 */
	void transitionActivated(int transId);
	/*! \brief Emitted whenever a new frame presentation is traveresed as part of playback.
	 *	\details The "time" input is the frame time.
	*/
	void framePresented(double time);
	/*! \brief Emitted when playback moves the current time passed a new reward value.  
	 *	\details time is the time that the reward was supplied, duration is its duraiton in milliseconds, 
	 *	channel is the reward channel on which the reward was supplied.
	 */
	void rewardSupplied(double time,int duration,int channel);
	/*! \brief Emitted whenever a Signal data sample time is passed as part of playback.
	 *	\details name is the name of the Signal channel, subChanNames is a list of the names of
	 *	that signals sub channels, vals is a Vector of the new float values of those signals, where
	 *	they are listed in the same order as subChanNames.  Values returned have already been calibrated
	 *	according to the experimental calibration parameters set in the remote viewer during the experiment.
	 */
	void signalChanged(QString name,QStringList subChanNames,QVector<float> vals);
	/*! \brief Emitted when playback moves the current time passed a new lfp value.  
	 *	\details channel is the channel on which the value occured, value is the value.
	 */
	void lfpChanged(int channel,double value);
	/*! \brief Emitted whenever a spike time is passed as part of playback.
	 *	\details time is the time at which the spike occured, channel is the electrode channel on which
	 *	it occured, unit is the neuron unit within that channel that spiked, and waveform is a vector of
	 *	floats describing the spike waveform.
	 */
	void spikeEvent(double time, int channel, int unit, QVector<float> waveform);
	/*! \brief During the session load process (started when setSessionData() is called), this
	 *	signal provides the current load percentage as a number between 0 and 100.
	 *	\note This is definitely not a linear scale with respect to time.  It just
	 *	provides a "general sense" of the current percent loaded.
	 */
	void percentLoaded(double percent);

private:
	void updatePercentLoaded();

	QSharedPointer<PropertyState> propState_;
	QSharedPointer<PropertyState> initPropState_;
	QSharedPointer<TransitionState> transState_;
	QSharedPointer<FrameState> frameState_;
	QSharedPointer<RewardState> rewardState_;
	QSharedPointer<RunNotesState> runNotesState_;
	QHash<QString,QSharedPointer<SignalState>> signalLookup_;
	QSharedPointer<SpikeState> spikeState_;
	QSharedPointer<LfpState> lfpState_;

	QList<QSharedPointer<DataState>> statesWithIds_;
	QList<QSharedPointer<DataState>> statesWithTimes_;
	double currRunStart_;
	double currRunEnd_;
	int loadedStates_;
	int totalSubStates_;
	double lfpPercent_;

private slots:
	void lfpLoadingUpdated(int percent);
};

}; //namespace Picto
#endif