#include "SessionState.h"
using namespace Picto;

/*! \brief Constructs a new SessionState
 *	\details enableLfp indicates if Local Field Potential data will be played back as part of this SessionState
 *	or if it will be disabled (ignored).  Disabling LFP data can lead to a significant savings in RAM and load time.
 *
 *	In this constructor all of the various DataState objects necessary to read in Session data and play it back are
 *	created.  Their signals that are used to inform the outside world of session events during playback are connecte
 *	up to this objects signals and various values are initialized.
 */
SessionState::SessionState(bool enableLfp) :
propState_(new PropertyState(false)),
initPropState_(new PropertyState(true)),
transState_(new TransitionState()),
frameState_(new FrameState()),
rewardState_(new RewardState()),
runNotesState_(new RunNotesState()),
spikeState_(new SpikeState()),
lfpState_(new LfpState(enableLfp))
{
	statesWithIds_.push_back(initPropState_);
	statesWithIds_.push_back(propState_);
	statesWithIds_.push_back(transState_);
	statesWithIds_.push_back(frameState_);

	statesWithTimes_.push_back(rewardState_);
	statesWithTimes_.push_back(spikeState_);
	statesWithTimes_.push_back(lfpState_);

	connect(propState_.data(),SIGNAL(propertyChanged(int,QString)),this,SIGNAL(propertyChanged(int,QString)));
	connect(initPropState_.data(),SIGNAL(propertyChanged(int,QString)),this,SIGNAL(propertyInitValueChanged(int,QString)));
	connect(transState_.data(),SIGNAL(transitionActivated(int)),this,SIGNAL(transitionActivated(int)));
	connect(frameState_.data(),SIGNAL(framePresented(double)),this,SIGNAL(framePresented(double)));
	connect(rewardState_.data(),SIGNAL(rewardSupplied(double,int,int)),this,SIGNAL(rewardSupplied(double,int,int)));
	connect(spikeState_.data(),SIGNAL(spikeEvent(double,int,int,QVector<float>)),this,SIGNAL(spikeEvent(double,int,int,QVector<float>)));
	connect(lfpState_.data(),SIGNAL(lfpChanged(int,double)),this,SIGNAL(lfpChanged(int,double)));
	connect(lfpState_.data(),SIGNAL(lfpLoadProgress(int)),this,SLOT(lfpLoadingUpdated(int)));

	currRunStart_ = currRunEnd_ = -1;
	loadedStates_ = 0;
	totalSubStates_ = 0;
	lfpPercent_ = 100;
}


SessionState::~SessionState()
{
}

/*! \brief Calls this function to load all session data into this SessionState.  
 *	\details session is the QSqlDatabase object from which all session data will be loaded.
 *	obsoleteAssetIds is a lookup table of Asset Ids for Assets that have become obsolete.
 *	When events relating to these Assets are encountered, they will be ignored.
 *	For more information on the obsoleteAssetIds lookup table, see the SessionVersionInterfacer object.  
 *	The SessionVersionInterfacer is used to prepare the input obsoleteAssetIds table.
 */
void SessionState::setSessionData(QSqlDatabase session,QHash<int,bool> obsoleteAssetIds)
{
	totalSubStates_ = statesWithIds_.size()+statesWithTimes_.size()+1;
	loadedStates_ = 0;
	lfpPercent_ = 100;
	currRunStart_ = currRunEnd_ = -1;
	propState_->setObsoleteAssets(obsoleteAssetIds);
	initPropState_->setObsoleteAssets(obsoleteAssetIds);
	transState_->setObsoleteAssets(obsoleteAssetIds);
	foreach(QSharedPointer<DataState> state,statesWithIds_)
	{
		state->setDatabase(session);
		loadedStates_++;
		updatePercentLoaded();
	}
	foreach(QSharedPointer<DataState> state,statesWithTimes_)
	{
		state->setDatabase(session);
		loadedStates_++;
		updatePercentLoaded();
	}
	runNotesState_->setDatabase(session);
	loadedStates_++;
	updatePercentLoaded();
}

/*! \brief Informs this SessionState that a new run is starting along with the time
 *	that the run is starting and the time that it will end (if available).  Times are
 *	in seconds.
 *	\details This is important not only because it initializes the current playback location to the
 *	beginning of the input run, but also because time values used in the Playback system are
 *	always with respect to the beginning of the run.  In otherwords, if
 *	something happens at the same time that the Run Starts, its time will be reported as zero, 
 *	even if the run happened at time 213.315.
 */
void SessionState::startNewRun(double startTime,double endTime)
{
	if(startTime < 0 || endTime < 0)
		return;
	foreach(QSharedPointer<DataState> state,statesWithIds_)
	{
		state->startRun(startTime,endTime);
	}
	foreach(QSharedPointer<DataState> state,statesWithTimes_)
	{
		state->startRun(startTime,endTime);
	}
	runNotesState_->startRun(startTime,endTime);
	currRunStart_ = startTime;
	currRunEnd_ = endTime;
}

/*! \brief Causes playback to restart from the beginning of the current run.
*/
void SessionState::restartRun()
{
	startNewRun(currRunStart_,currRunEnd_);
}

/*! \brief Adds a new SignalState to this SessionState in order to playback signal data for the 
 *	the signal with the input name.
 *	\details name is the name of the signal to be tracked by this LiveSignalReader (ie. "Position").
 *	tableName is the name of the table containing the signal data (ie. "signal_position"), subChanNames 
 *	is a list of the names of the subchannels of the signal to be tracked (ie. "x","y").  samplePeriod 
 *	is the time per sample for the signal channel.
 */
void SessionState::addSignal(QString name,QString tableName,QStringList subChanNames,double sampPeriod)
{
	if(!signalLookup_.contains(name))
	{
		QSharedPointer<SignalState> newSigState = QSharedPointer<SignalState>(new SignalState(name,tableName,subChanNames,sampPeriod));
		signalLookup_[name] = newSigState;
		statesWithTimes_.push_back(newSigState);
		connect(newSigState.data(),SIGNAL(signalChanged(QString,QStringList,QVector<float>)),this,SIGNAL(signalChanged(QString,QStringList,QVector<float>)));
		//connect(newSigState.data(),SIGNAL(needsData(PlaybackIndex,PlaybackIndex)),this,SLOT(needsSignalData(PlaybackIndex,PlaybackIndex)));
		//connect(newSigState.data(),SIGNAL(needsNextData(PlaybackIndex,bool)),this,SLOT(needsNextSignalData(PlaybackIndex,bool)));
	}
}

/*! \brief Enables/disables tracking of LFP data.
 *	\details Disabling LFP data when it isn't needed translates into savings in RAM and loadtime.
 *	\sa LfpState::setEnabled()
 */
void SessionState::enableLfp(bool enable)
{
	lfpState_->setEnabled(enable);
}

/*! \brief Returns true if Lfp tracking is enabled, false if it is disabled.
 *	\details Disabling LFP data when it isn't needed translates into savings in RAM and loadtime.
 *	\sa enableLfp(), LfpState::setEnabled()
 */
bool SessionState::lfpEnabled()
{
	if(!lfpState_)
		return false;
	return lfpState_->getEnabled();
}

/*! \brief Returns a FrameReader object that can be used to query Frame presentation data over time intervals.
 */
QSharedPointer<FrameReader> SessionState::getFrameReader()
{
	return frameState_;
}

/*! \brief Returns a RewardReader object that can be used to query reward supply data over time intervals.
 */
QSharedPointer<RewardReader> SessionState::getRewardReader()
{
	return rewardState_;
}

/*! \brief Returns a RunNotesReader object that can be used to query meta-data about the current Run.
 */
QSharedPointer<RunNotesReader> SessionState::getRunNotesReader()
{
	return runNotesState_;
}

/*! \brief Returns a list of all SignalReader names that are available in this SessionState.
 *	\details Each Signal channel has its own SignalReader and which of these channels are available is a
 *	function of the Director hardware and configuration used to run the Picto Session.
 */
QStringList SessionState::getSignalReaderNames()
{
	return signalLookup_.keys();
}

/*! \brief Returns a SignalReader object that can be used to query signal data over time intervals.
 *	\details The input name defines which SignalReader will be returned.  Valid SignalReader names
 *	can be accessed from getSignalReaderNames().
 */
QSharedPointer<SignalReader> SessionState::getSignalReader(QString name)
{
	if(signalLookup_.contains(name)) 
		return signalLookup_[name];
	return QSharedPointer<SignalState>();
}

/*! \brief Returns an LfpReader object that can be used to query Local Field Potential data over time intervals.
 *	\note If lfpEnabled() is false , this LfpReader will not contain any data regardless of whether LFP data was
 *	tracked during the session.  Lfp Enabling/Disabling is set in the SessionState constructor and in enabledLfp().
 */
QSharedPointer<LfpReader> SessionState::getLfpReader()
{
	return lfpState_;
}

/*! \brief Returns a SpikeReader object that can be used to query Spike times and data over time intervals.
 */
QSharedPointer<SpikeReader> SessionState::getSpikeReader()
{
	return spikeState_;
}

/*! \brief Returns a PropertyState object that can be used to playback Property value changes over time.
 */
QSharedPointer<PropertyState> SessionState::getPropertyState()
{
	return propState_;
}

/*! \brief Returns a TransitionState object that can be used to playback Transition traversal data over time.
 */
QSharedPointer<TransitionState> SessionState::getTransitionState()
{
	return transState_;
}

/*! \brief Returns a FrameState object that can be used to playback Frame presentation data over time.
 */
QSharedPointer<FrameState> SessionState::getFrameState()
{
	return frameState_;
}

/*! \brief Returns a RewardState object that can be used to playback Reward delivery data over time.
 */
QSharedPointer<RewardState> SessionState::getRewardState()
{
	return rewardState_;
}

/*! \brief Returns a SignalState object that can be used to playback Signal value data over time.
 *	\details The input name is the name of the signal tracked by the SignalState (ie. "Property").
 *	Use getSignalReaderNames() to see what names are available.
 */
QSharedPointer<SignalState> SessionState::getSignalState(QString name)
{
	if(signalLookup_.contains(name)) 
		return signalLookup_[name];
	return QSharedPointer<SignalState>();
}

/*! \brief Returns an LfpState object that can be used to playback Local Field Potential data over time.
 *	\note If lfpEnabled() is false , the returned LfpState will not contain any data regardless of whether LFP data was
 *	tracked during the session.  Lfp Enabling/Disabling is set in the SessionState constructor and in enabledLfp().
 */
QSharedPointer<LfpState> SessionState::getLfpState()
{
	return lfpState_;
}

/*! \brief Returns a SpikeState object that can be used to playback Spike data over time.
 */
QSharedPointer<SpikeState> SessionState::getSpikeState()
{
	return spikeState_;
}

/*! \brief Returns a list of all DataState objects included in this SessionState that are indexed
 *	with a DataId.
 *	\details Some Session data like Spike times or input signal times is indexed by its timestamp only.
 *	Other values, like Property changes and Transition traversals are indexed by time and a DataID since these
 *	values are marked as occuring at the the time of the next presented frame, so they need this extra
 *	means of capturing the ordering of changes within the frame.
 */
QList<QSharedPointer<DataState>> SessionState::getStatesIndexedById()
{
	return statesWithIds_;
}

/*! \brief Returns a list of all DataState objects included in this SessionState that are only indexed
 *	according to their timestamp.
 *	\details Some Session data like Spike times or input signal times is indexed by its timestamp only.
 *	Other values, like Property changes and Transition traversals are indexed by time and a DataID since these
 *	values are marked as occuring at the the time of the next presented frame, so they need this extra
 *	means of capturing the ordering of changes within the frame.
 */
QList<QSharedPointer<DataState>> SessionState::getStatesIndexedByTime()
{
	return statesWithTimes_;
}

/*! \brief Call this when something significant happens in the load process to update the total load precentage
 *	and report it over the percentLoaded() signal.
 *	\note This function simply looks at the number of DataState objects that have fully loaded their data and 
 *	compares that to the total number of DataStates.  The one exception to this is the LfpState.  Since the 
 *	LfpState takes the longest to load, it exposes an lfpLoadProgress() signal that provides a more fine
 *	grained picture of its load process.  That signal is used with lfpLoadingUpdated() which calls this function 
 *	for a fine grained load update.  This is useful so that the load process doesn't appear to freeze when it
 *	reaches the LfpState.
 *	\sa lfpLoadingUpdated()
 */
void SessionState::updatePercentLoaded()
{
	//Handle lfp separately because its so big and so that
	//a meaningful value can appear here when it is being loaded
	//on its own
	double currLoadedStates = loadedStates_;
	if(lfpPercent_ < 100)
	{
		if(loadedStates_ == totalSubStates_)
			currLoadedStates = loadedStates_-1;
		currLoadedStates += double(lfpPercent_)/100.0;
	}
	if(totalSubStates_ > 0)
		emit percentLoaded(100.0*currLoadedStates/double(totalSubStates_));
}

/*! \brief Called when the LfpState updates its load percentage.  Calls updatePercentLoaded() internally
 *	to update the Playback system of the load status with the percentLoaded() signal.
 *	\sa updatePercentLoaded(), LfpState::lfpLoadProgress()
 */
void SessionState::lfpLoadingUpdated(int percent)
{
	//We handle lfp loading differently because its so big
	lfpPercent_ = percent;
	updatePercentLoaded();
}