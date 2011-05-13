#ifndef _PropertyFactory_H_
#define _PropertyFactory_H_

#include "DataStoreFactory.h"
#include "../Property/PropertyContainer.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>
#include <QMap>

namespace Picto {

class DataStore;

#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyFactory : public DataStoreFactory
#else
class PropertyFactory : public DataStoreFactory
#endif
{
public:
	PropertyFactory(
		int minDataStores,
		int maxDataStores,
		QSharedPointer<PropertyContainer> propContainer,
		int propertyType, 
		QString propIdentifier, 
		QVector<QSharedPointer<DataStore>> defaultDataStores = QVector<QSharedPointer<DataStore>>()
		);
	
protected:
	virtual QSharedPointer<DataStore> generateNewDataStore();
private:
	QSharedPointer<PropertyContainer> propContainer_;
	int propertyType_;
	QString propIdentifier_;
};

}; //namespace Picto

#endif
