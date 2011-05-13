#include "PropertyFactory.h"

using namespace Picto;

PropertyFactory::PropertyFactory	(	
		int minDataStores,
		int maxDataStores,
		QSharedPointer<PropertyContainer> propContainer,
		int propertyType, 
		QString propIdentifier, 
		QVector<QSharedPointer<DataStore>> defaultDataStores

									) :
DataStoreFactory(minDataStores,maxDataStores,NULL,defaultDataStores),
propContainer_(propContainer),
propertyType_(propertyType),
propIdentifier_(propIdentifier)
{
}

QSharedPointer<DataStore> PropertyFactory::generateNewDataStore()
{
	return propContainer_->addProperty(propertyType_,propIdentifier_,QVariant());
}