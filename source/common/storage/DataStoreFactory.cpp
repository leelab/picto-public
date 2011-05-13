#include "DataStoreFactory.h"

using namespace Picto;

DataStoreFactory::DataStoreFactory(QSharedPointer<DataStore> singleDefaultDataStore) :
minDataStores_(1),
maxDataStores_(1),
newDataStoreFn_(NULL),
numSourcedDataStores_(0)
{
	defaultDataStores_.push_back(singleDefaultDataStore);
}

DataStoreFactory::DataStoreFactory	(	
	int minDataStores,
	int maxDataStores,
	NewDataStoreFnPtr newDataStoreFn, 
	QVector<QSharedPointer<DataStore>> defaultDataStores
									) :
minDataStores_(minDataStores),
maxDataStores_(maxDataStores),
newDataStoreFn_(newDataStoreFn),
defaultDataStores_(defaultDataStores),
numSourcedDataStores_(0)
{
}

void DataStoreFactory::addDataStoreType(QString type,QSharedPointer<DataStoreFactory> factory)
{
	factoriesByType_[type] = factory;
}

QList<QString> DataStoreFactory::getTypes()
{
	QList<QString> returnVal;
	returnVal.push_back("");
	returnVal.append(factoriesByType_.keys());
	return returnVal;
}

QSharedPointer<DataStore> DataStoreFactory::getDataStore(QString& error, QString type)
{
	if(type != "")
	{
		if(factoriesByType_.contains(type))
			return factoriesByType_.value(type)->getDataStore(error,"");
		error = "Factory does not contain type: " + type.toAscii();
		return QSharedPointer<DataStore>();
	}
	if((maxDataStores_ > -1) && numSourcedDataStores_ >= maxDataStores_)
	{
		error = "Num  created DataStores has exceeded the maximum: " + QString::number(maxDataStores_);
		return QSharedPointer<DataStore>();
	}
	numSourcedDataStores_++;
	if(numSourcedDataStores_ <= defaultDataStores_.size())
		return defaultDataStores_[numSourcedDataStores_-1];
	return generateNewDataStore();
}

QSharedPointer<DataStore> DataStoreFactory::generateNewDataStore()
{
	return QSharedPointer<DataStore>((*newDataStoreFn_)());
}

//Resets the number of data stores sourced so far
void DataStoreFactory::startSourcing()
{
	numSourcedDataStores_ = 0;
	foreach(QSharedPointer<DataStoreFactory> factory,factoriesByType_)
	{
		factory->startSourcing();
	}
}

//Returns QSharedPointers to datastores until all the requirements
//for minimum numbers of datastores in this factory have been met.
QSharedPointer<DataStore> DataStoreFactory::getRequiredDataStore()
{
	if(numSourcedDataStores_ < defaultDataStores_.size())
		return defaultDataStores_[numSourcedDataStores_++];
	QSharedPointer<DataStore> newDataStore;
	foreach(QSharedPointer<DataStoreFactory> factory,factoriesByType_)
	{
		newDataStore = factory->getRequiredDataStore();
		if(!newDataStore.isNull())
			break;
	}
	return newDataStore;
}

