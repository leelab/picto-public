#include "PropertyFactory.h"
#include "../memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a PropertyFactory object.
 *	@param minAssets This is here for historical reasons.  It should always just be 1.
 *	@param maxAssets This is here for historical reasons.  It should always just be 1.
 *	@param propContainer The PropertyContainer to which generated Property objects should be added.
 *	@param propertyType The type of the Property to be added.
 *	@param propIdentifier The name of the Property to be created.
 *	@param defaultValue The default value of the Property.
 *	@param attributeMap The attributes that should be added to the Property (this is used for EnumProperty).
 *	\sa AssetFactory::AssetFactory()
 */
PropertyFactory::PropertyFactory	(	
		int minAssets,
		int maxAssets,
		QSharedPointer<PropertyContainer> propContainer,
		int propertyType, 
		QString propIdentifier,
		QVariant defaultValue,
		QMap<QString,QVariant> attributeMap

									) :
AssetFactory(minAssets,maxAssets,NULL/*,defaultAssets*/),
propContainer_(propContainer),
propertyType_(propertyType),
propIdentifier_(propIdentifier),
defaultValue_(defaultValue),
attributeMap_(attributeMap)
{
}

/*! \brief Implements AssetFactory::generateNewAsset() to create a Property in the PropertyContainer of the correct type/value/etc according to the values
 *	passed into the constructor.
 */
QSharedPointer<Asset> PropertyFactory::generateNewAsset()
{
	QSharedPointer<Property> prop = propContainer_->addProperty(propertyType_,propIdentifier_,defaultValue_);
	if(!prop.isNull())
	{
		for(QMap<QString,QVariant>::iterator iter = attributeMap_.begin();iter != attributeMap_.end(); iter++)
		{
			prop->setAttribute(iter.key(),iter.value());
		}
	}
	return prop;
}