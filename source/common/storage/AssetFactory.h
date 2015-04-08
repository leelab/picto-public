#ifndef _ASSETFACTORY_H_
#define _ASSETFACTORY_H_

#include <QSharedPointer>
#include <QString>
#include <QVector>
#include <QMap>

#include "Asset.h"

namespace Picto {

/*! \brief A Factory class used to generate Asset objects.
 *
 *	\details Object of this class are used to generate Assets as part of the process of deserializing or constructing a
 *	Picto design.  The type of Asset that will be created is defined by passing a creation function pointer into the
 *	Factory constructor.  Requirements for the minimum number of Assets that must be created and the maximum that may be
 *	created can be set to the Factory.  The Factory also provides data about which UI templates, groups and documentation
 *	should be used for presenting the Assets generated with this Factory.
 *
 *	This class can also be used to group together sub-AssetFactory objects where similar types of Assets must all be
 *	created within a set of overall requirements.  To do this, a Group AssetFactory is created by using the constructor
 *	that doesn't include an input for a creation function pointer.  Sub-AssetFactories are then added by using
 *	addAssetType().  To create Assets of the sub types, use the getAsset() or getRequiredAsset() functions with an input
 *	type string that matches one of the types entered in addAssetType().  When a Group AssetFactory is used, the
 *	minimum/maximum requirements of the sub-AssetFactories must be met as well as those of the Group Factory.  In the case
 *	of the Group Factory, the minimum/maximum values apply to the overall total number of generated Assets of all
 *	sub-types.
 *
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AssetFactory : public QObject
#else
class AssetFactory
#endif
{
	Q_OBJECT
public:
	typedef QSharedPointer<Asset> (*NewAssetFnPtr)();
	AssetFactory(int minAssets = 0,int maxAssets = -1);
	AssetFactory(	int minAssets,int maxAssets,NewAssetFnPtr newAssetFn);
	virtual ~AssetFactory(){};
	void addAssetType(QString type,QSharedPointer<AssetFactory> factory);
	QStringList getTypes();

	QSharedPointer<Asset> getAsset(QString& error, QString type = "");
	void startSourcing();
	QSharedPointer<Asset> getRequiredAsset(QString& returnedType);
	bool reachedProductionLimit(QString type = "");
	int getMaxAssets(QString type = "");
	/*! \brief Sets the maximum number of Assets that may be generated by this factory.
	 *	\sa addAssetType()
	 */
	void setMaxAssets(int maxAssets){maxAssets_ = maxAssets;};
	int getMinAssets(QString type = "");
	/*! \brief Sets the minimum number of Assets that must be generated by this factory.
	 *	\sa addAssetType()
	 */
	void setMinAssets(int minAssets){minAssets_ = minAssets;};
	/*! \brief Returns the number of Assets generated by this Factory so far (since the last startSourcing() call).
	 */
	int getGeneratedAssets(){return numSourcedAssets_;};
	/*! \brief In exceptional cases where Assets need to be created outside the Factory for some reason but need to be
	 *	counted toward the maximum value, use this function to update the Factory with the number of Assets generated so
	 *	far.
	 */
	void setGeneratedAssets(int numGeneratedAssets){numSourcedAssets_ = numGeneratedAssets;};
	QString getUITemplate(QString type);
	QString getUIGroup(QString type);
	QString getGeneratedAssetTypeName(QString type);
	QString getGeneratedAssetClassName(QString type);
protected:
	virtual QSharedPointer<Asset> generateNewAsset();
private:
	QVector<QSharedPointer<Asset>> defaultAssets_;
	int minAssets_;
	int maxAssets_;
	int numSourcedAssets_;
	const bool isGroupFactory_;
	QString uITemplate_;
	QString uIGroup_;
	QString generatedAssetTypeName_;
	QString assetClassName_;
	bool uITemplateInitialized_;

	NewAssetFnPtr newAssetFn_;
	QMap<QString, QSharedPointer<AssetFactory>> factoriesByType_;
private slots:
	void createdAssetDestroyed(QObject* obj);
};

}; //namespace Picto

#endif
