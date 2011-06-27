#include "PropertyFactory.h"

using namespace Picto;

PropertyFactory::PropertyFactory	(	
		int minAssets,
		int maxAssets,
		QSharedPointer<PropertyContainer> propContainer,
		int propertyType, 
		QString propIdentifier,
		QVariant defaultValue,
		QMap<QString,QVariant> attributeMap/*,
		QVector<QSharedPointer<Asset>> defaultAssets*/

									) :
AssetFactory(minAssets,maxAssets,NULL/*,defaultAssets*/),
propContainer_(propContainer),
propertyType_(propertyType),
propIdentifier_(propIdentifier),
defaultValue_(defaultValue),
attributeMap_(attributeMap)
{
}

QSharedPointer<Asset> PropertyFactory::generateNewAsset()
{
	QSharedPointer<Property> prop = propContainer_->addProperty(propertyType_,propIdentifier_,defaultValue_,((getMaxAssets()==-1) || (getMaxAssets()>1)) );
	if(!prop.isNull())
	{
		for(QMap<QString,QVariant>::iterator iter = attributeMap_.begin();iter != attributeMap_.end(); iter++)
		{
			prop->addAttribute(iter.key(),iter.value());
		}
	}
	return prop;
}