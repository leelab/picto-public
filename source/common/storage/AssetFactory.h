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
	AssetFactory(int minAssets = 0,int maxAssets = -1);
	//AssetFactory(QSharedPointer<Asset> singleDefaultAsset);
	AssetFactory(	int minAssets,int maxAssets,NewAssetFnPtr newAssetFn/*,QVector<QSharedPointer<Asset>> defaultAssets = QVector<QSharedPointer<Asset>>()*/);
	void addAssetType(QString type,QSharedPointer<AssetFactory> factory);
	QStringList getTypes();

	QSharedPointer<Asset> getAsset(QString& error, QString type = "");
	//Resets the number of data stores sourced so far
	void startSourcing();
	//Returns QSharedPointers to Assets until all the requirements
	//for minimum numbers of Assets in this factory have been met.
	//The type of the returned asset is in the returnedType field.
	QSharedPointer<Asset> getRequiredAsset(QString& returnedType);
	bool reachedProductionLimit(QString type = "");
	int getMaxAssets(){return maxAssets_;};
	void setMaxAssets(int maxAssets){maxAssets_ = maxAssets;};
	int getMinAssets(){return minAssets_;};
	void setMinAssets(int minAssets){minAssets_ = minAssets;};
	int getGeneratedAssets(){return numSourcedAssets_;};
	void setGeneratedAssets(int numGeneratedAssets){numSourcedAssets_ = numGeneratedAssets;};
protected:
	virtual QSharedPointer<Asset> generateNewAsset();
private:
	QVector<QSharedPointer<Asset>> defaultAssets_;
	int minAssets_;
	int maxAssets_;
	int numSourcedAssets_;
	const bool isGroupFactory_;

	NewAssetFnPtr newAssetFn_;
	QMap<QString, QSharedPointer<AssetFactory>> factoriesByType_;
};

}; //namespace Picto

#endif