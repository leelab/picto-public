#ifndef _EXPERIMENTCONFIG_H_
#define _EXPERIMENTCONFIG_H_

#include "../common.h"
#include "serializable.h"
#include <QObject>
#include <QMap>
#include <QSet>
#include <QWeakPointer>
namespace Picto {
class Asset;
struct AssetInfo{int id;QString path;};
struct PropInfo{int id; QString name; int parent;};
struct TransInfo{int id; QString name; int parent; QString source;QString result;QString dest;};

#if defined WIN32 || defined WINCE
class PICTOLIB_API ExperimentConfig : public QObject, public Serializable
#else
class ExperimentConfig : public QObject, public Serializable
#endif
{
	Q_OBJECT
public:
	ExperimentConfig();
	virtual ~ExperimentConfig(){};

	using Serializable::toXml;
	using Serializable::fromXml;
	bool toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	void disallowIdDuplication();
	void addManagedAsset(QSharedPointer<Asset> asset);
	void fixDuplicatedAssetIds();
	QSharedPointer<Asset> getAsset(int id);
	QList<QWeakPointer<Asset>> getProperties(){return managedProperties_;};

	QList<AssetInfo> getElementInfo();
	QList<PropInfo> getPropertyInfo();
	QList<TransInfo> getTransitionInfo();
protected:
	
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
	int lastUsedId_;
	bool allowIdDuplication_;


};

}; //namespace Picto

#endif
