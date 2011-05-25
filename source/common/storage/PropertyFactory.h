#ifndef _PROPERTYFACTORY_H_
#define _PROPERTYFACTORY_H_

#include "AssetFactory.h"
#include "../Property/PropertyContainer.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>
#include <QMap>

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyFactory : public AssetFactory
#else
class PropertyFactory : public AssetFactory
#endif
{
public:
	PropertyFactory(
		int minAssets,
		int maxAssets,
		QSharedPointer<PropertyContainer> propContainer,
		int propertyType, 
		QString propIdentifier,
		QVariant defaultValue = QVariant(),
		QMap<QString,QVariant> attributeMap = QMap<QString,QVariant>(),
		QVector<QSharedPointer<Asset>> defaultAssets = QVector<QSharedPointer<Asset>>()
		);
	
protected:
	virtual QSharedPointer<Asset> generateNewAsset();
private:
	QSharedPointer<PropertyContainer> propContainer_;
	int propertyType_;
	QString propIdentifier_;
	QMap<QString,QVariant> attributeMap_;
	QVariant defaultValue_;
};

}; //namespace Picto

#endif
