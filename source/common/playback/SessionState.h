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
#include "TransitionState.h"
#include "PlaybackInterfaces.h"
#include "../common.h"

namespace Picto {
	typedef enum{eProperty,eTransition,eFrame,eReward,eSignal,eLfp,eSpike} PlaybackDataType;
/*! \brief Component of Picto Playback system that stores property values and transition states.
 *	Whenever property or transition values are changed, a corresponding signal is triggered.
 *	On the first iteration, this object will simply store current values.  Later, it may store 
 *	values according to time such that rewind can be sped up.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SessionState : public QObject
#else
class SessionState : public  QObject
#endif
{
	Q_OBJECT
public:
	SessionState();
	virtual ~SessionState();
	//SET FUNCTIONS-----------------------------------------------------------------
	void setSessionData(QSqlDatabase session,QHash<int,bool> obsoleteAssetIds = QHash<int,bool>());
	void startNewRun(double startTime,double endTime=-1);
	void restartRun();
	//Used to load values
	//bool reset();
	//Used to add a new signal to the SessionState since different sessions
	//can contain different numbers of signals.
	void addSignal(QString name,QString tableName,QStringList subChanNames,double sampPeriod);
	//Used to add a new lfp channel to the SessionState since different sessions
	//can contain different numbers of lfp channels and each is tracked seperately.
	void addLfpChannel(int channel,double sampPeriod);
	//The before boolean should be set true if data will be inserted in decreasing index order
	//before the current data set.
	//bool setPropertyValue(double time,qulonglong dataId,int propId,QString value);
	//bool setTransition(double time,qulonglong dataId,int transId);
	//bool setFrame(qulonglong dataId,double frameTime);
	//bool setReward(double time,qulonglong dataId,int duration,int channel);
	//bool setSignal(QString name,QStringList subChanNames,double time,qulonglong dataId,double sampPeriod,QByteArray data);
	//bool setLFP(qulonglong dataId,double startTime,double sampPeriod,int channel,QByteArray data);
	//bool setSpike(qulonglong dataId,double spikeTime,int channel,int unit,QByteArray waveform);

	//void setBehavioralBounds(double min,double max);
	//void clearBehavioralData(double bound,bool before);
	//void setBehavioralFinished();
	//void setNeuralBounds(double min,double max);
	//void clearNeuralData(double bound,bool before);
	//void setNeuralFinished();


	//GET FUNCTIONS-----------------------------------------------------------------
	//The separate types of DataReaders are used to access asynchronous data
	//such as spikes since 1 second ago, lfp data until 500ms from now, etc.
	QSharedPointer<PropertyReader> getPropertyReader();
	QSharedPointer<TransitionReader> getTransitionReader();
	QSharedPointer<FrameReader> getFrameReader();
	QSharedPointer<RewardReader> getRewardReader();
	QStringList getSignalReaderNames();
	QSharedPointer<SignalReader> getSignalReader(QString name);
	QSharedPointer<LfpReader> getLfpReader(int channel);
	QSharedPointer<SpikeReader> getSpikeReader();

	//The separate types of DataStates are used to step through the session.
	QSharedPointer<PropertyState> getPropertyState();
	QSharedPointer<TransitionState> getTransitionState();
	QSharedPointer<FrameState> getFrameState();
	QSharedPointer<RewardState> getRewardState();
	QSharedPointer<SignalState> getSignalState(QString name);
	QSharedPointer<LfpState> getLfpState(int channel);
	QSharedPointer<SpikeState> getSpikeState();

	QList<QSharedPointer<DataState>> getStatesIndexedById();
	QList<QSharedPointer<DataState>> getStatesIndexedByTime();
signals:
	
	//Synchronous Data Signals, triggered when values change
	void propertyChanged(int propId, QString value);
	void transitionActivated(int transId);
	void framePresented(double time);
	void rewardSupplied(double time,int duration,int channel);
	void signalChanged(QString name,QStringList subChanNames,QVector<float> vals);
	void lfpChanged(int channel,double value);
	void spikeEvent(int channel, int unit, QVector<float> waveform);
	void percentLoaded(double percent);

	//Indicates that data is needed by the session state to fulfil playback requests
	//from (not including) currLast up to and including "to" of type "type".  
	//If data is not available, the system should wait for it to arrive before 
	//returning from this function.  More data than requested may be supplied, 
	//but it must not be before or at the currLast index. All data for a given time 
	//should be supplied at the same time (ie. if spikes from two channels came in 
	//at the same time, they should both be supplied from the same needsData call).
	//void needsData(PlaybackDataType type,PlaybackIndex currLast,PlaybackIndex to);

	//Indicates that at least a single data unit is needed by the session state 
	//to fulfil playback requests from (not including) currLast in the direction of
	//the backward indicator of type "type".  If data is not available, the system 
	//should wait for it to arrive before returning from this function.  
	//More data than requested may be supplied, but it must not be before or at the
	//currLast index. 
	//void needsNextData(PlaybackDataType type,PlaybackIndex currLast,bool backward);

	//Indicates that this SessionState was reset (for a new Run for example)
	void wasReset();

private:

	//Lookup table of Property States indexed by PropId used for reversing and 
	QSharedPointer<PropertyState> propState_;
	QSharedPointer<TransitionState> transState_;
	QSharedPointer<FrameState> frameState_;
	QSharedPointer<RewardState> rewardState_;
	QHash<QString,QSharedPointer<SignalState>> signalLookup_;
	QHash<int,QSharedPointer<LfpState>> lfpLookup_;
	QSharedPointer<SpikeState> spikeState_;

	QList<QSharedPointer<DataState>> statesWithIds_;
	QList<QSharedPointer<DataState>> statesWithTimes_;
	double currRunStart_;
	double currRunEnd_;

//private slots:
//	void needsPropertyData(PlaybackIndex currLast,PlaybackIndex to);
//	void needsTransitionData(PlaybackIndex currLast,PlaybackIndex to);
//	void needsFrameData(PlaybackIndex currLast,PlaybackIndex to);
//	void needsRewardData(PlaybackIndex currLast,PlaybackIndex to);
//	void needsSignalData(PlaybackIndex currLast,PlaybackIndex to);
//	void needsLFPData(PlaybackIndex currLast,PlaybackIndex to);
//	void needsSpikeData(PlaybackIndex currLast,PlaybackIndex to);
//
//	void needsNextPropertyData(PlaybackIndex currLast,bool backward);
//	void needsNextTransitionData(PlaybackIndex currLast,bool backward);
//	void needsNextFrameData(PlaybackIndex currLast,bool backward);
//	void needsNextRewardData(PlaybackIndex currLast,bool backward);
//	void needsNextSignalData(PlaybackIndex currLast,bool backward);
//	void needsNextLFPData(PlaybackIndex currLast,bool backward);
//	void needsNextSpikeData(PlaybackIndex currLast,bool backward);

};

}; //namespace Picto
#endif