#ifndef _SESSIONSTATE_H_
#define _SESSIONSTATE_H_
#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include <QVector>
#include "FrameState.h"
#include "LfpState.h"
#include "PropertyCollectionState.h"
#include "RewardState.h"
#include "SignalState.h"
#include "SpikeState.h"
#include "TransitionState.h"
#include "PlaybackInterfaces.h"
#include "../common.h"

namespace Picto {
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
	//Used to load values
	bool reset();
	//The before boolean should be set true if data will be inserted in decreasing index order
	//before the current data set.
	bool setPropertyValue(double time,qulonglong dataId,int propId,QString value);
	bool setTransition(double time,qulonglong dataId,int transId);
	bool setFrame(qulonglong dataId,double frameTime);
	bool setReward(double time,qulonglong dataId,int duration,int channel);
	bool setSignal(QString name,QStringList subChanNames,double time,qulonglong dataId,double sampPeriod,QByteArray data);
	bool setLFP(qulonglong dataId,double startTime,int channel,QByteArray data);
	bool setSpike(qulonglong dataId,double spikeTime,int channel,int unit,QByteArray waveform);

	//GET FUNCTIONS-----------------------------------------------------------------
	//The separate types of DataReaders are used to access asynchronous data
	//such as spikes since 1 second ago, lfp data until 500ms from now, etc.
	QSharedPointer<PropertyReader> getPropertyReader(int propId);
	QSharedPointer<TransitionReader> getTransitionReader();
	QSharedPointer<FrameReader> getFrameReader();
	QSharedPointer<RewardReader> getRewardReader();
	QSharedPointer<SignalReader> getSignalReader(QString name);
	QSharedPointer<LfpReader> getLfpReader();
	QSharedPointer<SpikeReader> getSpikeReader();

	//The separate types of DataStates are used to step through the session.
	QSharedPointer<PropertyCollectionState> getPropertyCollectionState();
	QSharedPointer<TransitionState> getTransitionState();
	QSharedPointer<FrameState> getFrameState();
	QSharedPointer<RewardState> getRewardState();
	QSharedPointer<SignalState> getSignalState(QString name);
	QSharedPointer<LfpState> getLfpState();
	QSharedPointer<SpikeState> getSpikeState();

	QList<QSharedPointer<DataState<qulonglong>>> getStatesIndexedById();
	QList<QSharedPointer<DataState<double>>> getStatesIndexedByTime();
signals:
	
	//Synchronous Data Signals, triggered when values change
	void propertyChanged(int propId, QString value);
	void transitionActivated(int transId);
	void framePresented(double time);
	void rewardSupplied(int duration,int channel);
	void signalChanged(QString name,QVector<float> vals);
	void lfpChanged(int channel,double value);
	void spikeEvent(int channel, int unit, QVector<float> waveform);

	//Indicates that data is needed by the session state to fulfil playback requests
	//from boundIndex up to and including toIndex.  More data may be supplied.  This
	//will improve speed at the expense of memory space.
	void needsPropertyData(qulonglong boundIndex,qulonglong toIndex);
	void needsTransitionData(qulonglong boundIndex,qulonglong toIndex);
	void needsFrameData(qulonglong boundIndex,qulonglong toIndex);
	void needsRewardData(double boundTime,double toTime);
	void needsSignalData(double boundTime,double toTime);
	void needsLFPData(double boundTime,double toTime);
	void needsSpikeData(double boundTime,double toTime);
	//Indicates that this SessionState was reset (for a new Run for example)
	void wasReset();

private:

	//Lookup table of Property States indexed by PropId used for reversing and 
	QSharedPointer<PropertyCollectionState> propCollection_;
	QSharedPointer<TransitionState> transState_;
	QSharedPointer<FrameState> frameState_;
	QSharedPointer<RewardState> rewardState_;
	QHash<QString,QSharedPointer<SignalState>> signalLookup_;
	QSharedPointer<LfpState> lfpState_;
	QSharedPointer<SpikeState> spikeState_;

	QList<QSharedPointer<DataState<qulonglong>>> statesWithIds_;
	QList<QSharedPointer<DataState<double>>> statesWithTimes_;

};

}; //namespace Picto
#endif