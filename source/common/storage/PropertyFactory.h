#ifndef _PROPERTYFACTORY_H_
#define _PROPERTYFACTORY_H_

#include "SerializableFactory.h"
#include "../Property/PropertyContainer.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>
#include <QMap>

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyFactory : public SerializableFactory
#else
class PropertyFactory : public SerializableFactory
#endif
{
public:
	PropertyFactory(
		int minSerializables,
		int maxSerializables,
		QSharedPointer<PropertyContainer> propContainer,
		int propertyType, 
		QString propIdentifier,
		QVariant defaultValue = QVariant(),
		QMap<QString,QVariant> attributeMap = QMap<QString,QVariant>(),
		QVector<QSharedPointer<Serializable>> defaultSerializables = QVector<QSharedPointer<Serializable>>()
		);
	
protected:
	virtual QSharedPointer<Serializable> generateNewSerializable();
private:
	QSharedPointer<PropertyContainer> propContainer_;
	int propertyType_;
	QString propIdentifier_;
	QMap<QString,QVariant> attributeMap_;
	QVariant defaultValue_;
};

}; //namespace Picto

#endif
