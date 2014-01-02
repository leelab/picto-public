#ifndef _PROPERTYFACTORY_H_
#define _PROPERTYFACTORY_H_

#include <QSharedPointer>
#include <QString>
#include <QVector>
#include <QMap>

#include "AssetFactory.h"
#include "../Property/PropertyContainer.h"

namespace Picto {

/*! \brief Since Property objects are just another kind of Asset, we pickyback onto the AssetFactory system to create the Property objects used in DataStores.
 *
 *	\details A Property AssetFactory has minAssets and maxAsset of 1.  It is slightly more complicated than a typical AssetFactory since generating
 *	a Property requires calling a non-static function on a particular PropertyContainer instance.  For that reason, this class simply extends AssetFactory
 *	and reimplements generateNewAsset() to make use of the dynamic addProperty() function on the PropertyContainer passed into the constructor.
 *  Other than that, it functions exactly the same.
 *	
 *	\note The PropertyFactory constructor includes minAsset, maxAssets inputs but it really shouldn't.  These should both always be one and it would be a
 *	good idea to fix this at some point.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
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
		QMap<QString,QVariant> attributeMap = QMap<QString,QVariant>()
		);
	virtual ~PropertyFactory(){};
	
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
