#ifndef _STATEUPDATER_H_
#define _STATEUPDATER_H_
#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>

#include "../common.h"
#include "../playback/PlaybackInterfaces.h"

namespace Picto {

/*! \brief An interface class for objects that update the Picto State from some external source.
 *	\details The source might be a remotely running experiment or an old session file that is being replayed.
 *	Either way, the StateUpdater communicates by emitting signals indicating that various events occured, in the order that they occured.
 *	It also provides various data reader objects that can be used to gather lists of past data
 *	or future data (in the case of experiments that are being played back from a previously saved session).
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	/*! \brief Calling this function updates the picto state to the latest available values.
	 *	\details The various StateUpdater signals will be triggered from within this function.
	 */
	virtual bool updateState() = 0;

	/*! \brief Returns a pointer to a FrameReader object that can be used to gather lists of Frame timing data.*/
	virtual QSharedPointer<FrameReader> getFrameReader() = 0;
	/*! \brief Returns a pointer to a LfpReader object that can be used to gather lists of lfp timing/value data.*/
	virtual QSharedPointer<LfpReader> getLfpReader() = 0;
	/*! \brief Returns a pointer to a RewardReader object that can be used to gather lists of reward data.*/
	virtual QSharedPointer<RewardReader> getRewardReader() = 0;
	/*! \brief Returns a pointer to a RunNotesReader object that can be used to gather data about the current run notes.*/
	virtual QSharedPointer<RunNotesReader> getRunNotesReader() = 0;
	/*! \brief Returns a pointer to a SpikeReader object that can be used to gather lists of spike data.*/
	virtual QSharedPointer<SpikeReader> getSpikeReader() = 0;
	/*! \brief Returns a list of SignalChannel names that are used in this experiment.*/
	virtual QStringList getSignalReaderNames() = 0;
	/*! \brief Returns a pointer to the SignalReader object with the input name that can be used to gather lists of signal data.*/
	virtual QSharedPointer<SignalReader> getSignalReader(QString name) = 0;
signals:
	/*! \brief Emitted when a new Run starts.  taskName is the name of the Task that will be run, runName is the name of this particular run as it
	 *	appears in the Run Notes tab of the Workstation.
	 */
	void startingRun(QString taskName,QString runName);
	/*! \brief Emitted when a run ends.*/
	void endingRun();
	//triggered when values change
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
	/*! \brief Emitted when rendering should be disabled/enabled.  
	 *	\details This is useful if we are skipping around in the run.  In this case all signals will still be correctly emitted in the correct
	 *	order and all data will be available on the data readers; however, the visual elements need not be displayed.
	 */
	void disableRendering(bool disable);
};


}; //namespace Picto

#endif
