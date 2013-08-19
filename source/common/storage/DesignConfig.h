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
struct AssetInfo{int id;QString path;};
struct PropInfo{int id; QString name; int parent;};
struct TransInfo{int id; QString name; int parent; QString source;QString result;QString dest;};

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
	QList<QWeakPointer<Asset>> getProperties(){return managedProperties_;};
	QList<QWeakPointer<Asset>> getAssets();
	QList<QWeakPointer<Asset>> getTransitions(){return managedTransitions_;};
	void setDeserializedVersion(QString version){deserializedVersion_ = version;};
	QString getDeserializedVersion(){return deserializedVersion_;};
	void setActiveAnalysisIds(QList<QUuid> analysisList);
	QList<QUuid> getActiveAnalysisIds();
	bool isAnalysisActive(QUuid analysisId);
	void markRunStart(QString runName);
	void markRunEnd();
	QUuid getLatestRunId();
	QString getLatestRunName();
	QSharedPointer<Controller::FrameTimerFactory> getFrameTimerFactory(){return frameTimerFactory_;};
	
	//Analysis Data Readers
	void setFrameReader(QSharedPointer<FrameReader> frameReader){frameReader_ = frameReader;};
	QSharedPointer<FrameReader> getFrameReader(){return frameReader_;};
	void setRewardReader(QSharedPointer<RewardReader> rewardReader){rewardReader_ = rewardReader;};
	QSharedPointer<RewardReader> getRewardReader(){return rewardReader_;};

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
	QSharedPointer<RewardReader> rewardReader_;

	QHash<QUuid,bool> analysisHash_;
	int lastUsedId_;
	bool allowIdDuplication_;
	QString deserializedVersion_;
	QUuid latestRunId_;
	QString latestRunName_;


};

}; //namespace Picto

#endif
