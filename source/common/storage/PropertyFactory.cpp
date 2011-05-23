#include "PropertyFactory.h"

using namespace Picto;

PropertyFactory::PropertyFactory	(	
		int minSerializables,
		int maxSerializables,
		QSharedPointer<PropertyContainer> propContainer,
		int propertyType, 
		QString propIdentifier,
		QVariant defaultValue,
		QMap<QString,QVariant> attributeMap,
		QVector<QSharedPointer<Serializable>> defaultSerializables

									) :
SerializableFactory(minSerializables,maxSerializables,NULL,defaultSerializables),
propContainer_(propContainer),
propertyType_(propertyType),
propIdentifier_(propIdentifier),
defaultValue_(defaultValue),
attributeMap_(attributeMap)
{
}

QSharedPointer<Serializable> PropertyFactory::generateNewSerializable()
{
	QSharedPointer<Property> prop = propContainer_->addProperty(propertyType_,propIdentifier_,defaultValue_,((getMaxSerializables()==-1) || (getMaxSerializables()>1)) );
	if(!prop.isNull())
	{
		for(QMap<QString,QVariant>::iterator iter = attributeMap_.begin();iter != attributeMap_.end(); iter++)
		{
			prop->addAttribute(iter.key(),iter.value());
		}
	}
	return prop;
}