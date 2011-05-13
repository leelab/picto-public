#ifndef _DATASTOREFACTORY_H_
#define _DATASTOREFACTORY_H_

#include "DataStore.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>
#include <QMap>

namespace Picto {

class DataStore;

#if defined WIN32 || defined WINCE
	class PICTOLIB_API DataStoreFactory
#else
class DataStoreFactory
#endif
{
public:
	typedef QSharedPointer<DataStore> (*NewDataStoreFnPtr)();
	DataStoreFactory(QSharedPointer<DataStore> singleDefaultDataStore);
	DataStoreFactory(	int minDataStores,int maxDataStores,NewDataStoreFnPtr newDataStoreFn = NULL,QVector<QSharedPointer<DataStore>> defaultDataStores = QVector<QSharedPointer<DataStore>>());
	void addDataStoreType(QString type,QSharedPointer<DataStoreFactory> factory);
	QList<QString> getTypes();

	QSharedPointer<DataStore> getDataStore(QString& error, QString type = "");
	//Resets the number of data stores sourced so far
	void startSourcing();
	//Returns QSharedPointers to datastores until all the requirements
	//for minimum numbers of datastores in this factory have been met.
	QSharedPointer<DataStore> getRequiredDataStore();
protected:
	virtual QSharedPointer<DataStore> generateNewDataStore();
private:
	QVector<QSharedPointer<DataStore>> defaultDataStores_;
	int minDataStores_;
	int maxDataStores_;
	int numSourcedDataStores_;

	NewDataStoreFnPtr newDataStoreFn_;
	QMap<QString, QSharedPointer<DataStoreFactory>> factoriesByType_;
};

}; //namespace Picto

#endif
