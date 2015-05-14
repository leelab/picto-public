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

public slots:
	//! Updates the lastTimeStateDataRequested_ if it's passed a more recent time.
	void updateCurrUnitTime(QString time);
	//! Prepares the RemoteStateUpdater to submit a processQueue signal after processing data
	void prepareToProcessQueue();

private:
	CommandChannel *serverChan_;
	QString lastTimeStateDataRequested_;
	QMap<QString, QSharedPointer<RemoteStateXMLHandler>> xmlHandlers_;
	bool sendProcessQueueSignal_;
};


}; //namespace Picto

#endif
