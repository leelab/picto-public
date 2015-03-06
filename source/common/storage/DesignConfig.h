#ifndef _DesignConfig_H_
#define _DesignConfig_H_

#include <QObject>
#include <QMap>
#include <QSet>
#include <QWeakPointer>
#include <QUuid>
#include "../common.h"
#include "serializable.h"

#include "../playback/PlaybackInterfaces.h"
#include "../controlelements/FrameTimerFactory.h"
namespace Picto {
class Asset;
/*! \brief Stores data about an Asset including its AssetId and Path in the Design.*/
struct AssetInfo{int id;QString path;};
/*! \brief Stores data about a Property including its AssetId, name, and its Parent Asset's AssetId.*/
struct PropInfo{int id; QString name; int parent;};
/*! \brief Stores data about a Transition including its AssetId, its Parent Asset's AssetId, its source, sourceResult and Destination names.*/
struct TransInfo{int id; QString name; int parent; QString source;QString result;QString dest;};

/*! \brief There are many operations and data elements that need to be associated with a design as a whole, accessable by all elements of the design
 *	and with access to all elements of a design.  Objects of the DesignConfig class handle these types of operations and store this type of data.
 *	
 *	\details Some of the types of operations that require access to/from all design Assets are: 
 *	- Making sure that all AssetIds are unique
 *	- Implementing Asset lookup based on its AssetId
 *	- Activating particular Analyses
 *	- Creating lists of all design Asset information for use in building up a SQL session file.
 *	- Storing objects for accessing data from input sources to be used by Experimental and Analysis components of the State Machine.
 *	These and other general design configuration issues are handled by this class.  Every Picto Asset holds a pointer to the DesignConfig of the
 *	design to which it is attached and adds itself to the DesignConfig object (which holds a weak pointer to it to avoid circular shared pointer issues).
 *	The DesignConfig of a particular Experiment or overall PictoData root object can be accessed using its Asset::getDesignConfig() function.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API DesignConfig : public QObject, public Serializable
#else
class DesignConfig : public QObject, public Serializable
#endif
{
	Q_OBJECT
public:
	DesignConfig();
	virtual ~DesignConfig(){};

	using Serializable::toXml;
	using Serializable::fromXml;
	void reset();
	bool toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	void disallowIdDuplication();
	void addManagedAsset(QSharedPointer<Asset> asset);
	void fixDuplicatedAssetIds();
	QSharedPointer<Asset> getAsset(int id);
	/*! \brief Returns a list (of weak pointers to) all Properties that were added to this DesignConfig.*/
	QList<QWeakPointer<Asset>> getProperties(){return managedProperties_;};
	QList<QWeakPointer<Asset>> getAssets();
	/*! \brief Returns a list (of weak pointers to) all Transitions that were added to this DesignConfig.*/
	QList<QWeakPointer<Asset>> getTransitions(){return managedTransitions_;};
	/*! \brief Sets the syntax version in which the design that is currently associated with this DesignConfig was serialized.*/
	void setDeserializedVersion(QString version){deserializedVersion_ = version;};
	/*! \brief Returns the syntax version in which the design that is currently associated with this DesignConfig was serialized.*/
	QString getDeserializedVersion(){return deserializedVersion_;};
	void setActiveAnalysisIds(QList<QUuid> analysisList);
	QList<QUuid> getActiveAnalysisIds();
	bool isAnalysisActive(QUuid analysisId);
	void markRunStart(QString runName);
	void markRunEnd();
	QUuid getLatestRunId();
	QString getLatestRunName();
	QSharedPointer<Controller::FrameTimerFactory> getFrameTimerFactory();
	
	//Analysis Data Readers
	void setFrameReader(QSharedPointer<FrameReader> frameReader);
	QSharedPointer<FrameReader> getFrameReader();
	void setLfpReader(QSharedPointer<LfpReader> lfpReader);
	void setRewardReader(QSharedPointer<RewardReader> rewardReader);
	void setRunNotesReader(QSharedPointer<RunNotesReader> runNotesReader);
	void setSpikeReader(QSharedPointer<SpikeReader> spikeReader);
	QSharedPointer<LfpReader> getLfpReader();
	QSharedPointer<RewardReader> getRewardReader();
	QSharedPointer<RunNotesReader> getRunNotesReader();
	QSharedPointer<SpikeReader> getSpikeReader();
	void setSignalReader(QString name, QSharedPointer<SignalReader> signalReader);
	void clearSignalReaders();
	QSharedPointer<SignalReader> getSignalReader(QString name);

	QList<AssetInfo> getElementInfo();
	QList<PropInfo> getPropertyInfo();
	QList<TransInfo> getTransitionInfo();
signals:
	void activeAnalysisIdsChanged();
	void runStarted(QUuid runId);
	void runEnded();
protected:
	void addError(QString errorMessage);
private:
	int getNewDataId();
	void addAsset(QSharedPointer<Asset> asset);
	QMap<int,QWeakPointer<Asset>> assetsById_;
	QList<QWeakPointer<Asset>> managedElements_;
	QList<QWeakPointer<Asset>> managedProperties_;
	QList<QWeakPointer<Asset>> managedTransitions_;

	QList<AssetInfo> elemInfo_;
	QList<PropInfo> propInfo_;
	QList<TransInfo> transInfo_;
	QHash<Asset*,QWeakPointer<Asset>> assetHash_;
	QList<QWeakPointer<Asset>> unsortedIdAssets_;

	QSharedPointer<Controller::FrameTimerFactory> frameTimerFactory_;

	QSharedPointer<FrameReader> frameReader_;
	QSharedPointer<LfpReader> lfpReader_;
	QSharedPointer<RewardReader> rewardReader_;
	QSharedPointer<RunNotesReader> runNotesReader_;
	QSharedPointer<SpikeReader> spikeReader_;
	QHash<QString,QSharedPointer<SignalReader>> signalReaders_;

	QHash<QUuid,bool> analysisHash_;
	int lastUsedId_;
	bool allowIdDuplication_;
	QString deserializedVersion_;
	QUuid latestRunId_;
	QString latestRunName_;


};

}; //namespace Picto

#endif
