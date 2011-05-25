#ifndef _ASSETFACTORY_H_
#define _ASSETFACTORY_H_

#include "Asset.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>
#include <QMap>

namespace Picto {

#if defined WIN32 || defined WINCE
	class PICTOLIB_API AssetFactory
#else
class AssetFactory
#endif
{
public:
	typedef QSharedPointer<Asset> (*NewAssetFnPtr)();
	AssetFactory();
	AssetFactory(QSharedPointer<Asset> singleDefaultAsset);
	AssetFactory(	int minAssets,int maxAssets,NewAssetFnPtr newAssetFn = NULL,QVector<QSharedPointer<Asset>> defaultAssets = QVector<QSharedPointer<Asset>>());
	void addAssetType(QString type,QSharedPointer<AssetFactory> factory);
	QList<QString> getTypes();

	QSharedPointer<Asset> getAsset(QString& error, QString type = "");
	//Resets the number of data stores sourced so far
	void startSourcing();
	//Returns QSharedPointers to Assets until all the requirements
	//for minimum numbers of Assets in this factory have been met.
	QSharedPointer<Asset> getRequiredAsset();
	int getMaxAssets(){return maxAssets_;};
	int getMinAssets(){return minAssets_;};
protected:
	virtual QSharedPointer<Asset> generateNewAsset();
private:
	QVector<QSharedPointer<Asset>> defaultAssets_;
	int minAssets_;
	int maxAssets_;
	int numSourcedAssets_;

	NewAssetFnPtr newAssetFn_;
	QMap<QString, QSharedPointer<AssetFactory>> factoriesByType_;
};

}; //namespace Picto

#endif
