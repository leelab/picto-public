#include "SerializableFactory.h"

using namespace Picto;
SerializableFactory::SerializableFactory() :
minSerializables_(0),
maxSerializables_(0),
newSerializableFn_(NULL),
numSourcedSerializables_(0)
{
}
SerializableFactory::SerializableFactory(QSharedPointer<Serializable> singleDefaultSerializable) :
minSerializables_(1),
maxSerializables_(1),
newSerializableFn_(NULL),
numSourcedSerializables_(0)
{
	defaultSerializables_.push_back(singleDefaultSerializable);
}

SerializableFactory::SerializableFactory	(	
	int minSerializables,
	int maxSerializables,
	NewSerializableFnPtr newSerializableFn, 
	QVector<QSharedPointer<Serializable>> defaultSerializables
									) :
minSerializables_(minSerializables),
maxSerializables_(maxSerializables),
newSerializableFn_(newSerializableFn),
defaultSerializables_(defaultSerializables),
numSourcedSerializables_(0)
{
}

void SerializableFactory::addSerializableType(QString type,QSharedPointer<SerializableFactory> factory)
{
	factoriesByType_[type] = factory;
}

QList<QString> SerializableFactory::getTypes()
{
	QList<QString> returnVal;
	returnVal.push_back("");
	returnVal.append(factoriesByType_.keys());
	return returnVal;
}

QSharedPointer<Serializable> SerializableFactory::getSerializable(QString& error, QString type)
{
	if(type != "")
	{
		if(factoriesByType_.contains(type))
			return factoriesByType_.value(type)->getSerializable(error,"");
		error = "Factory does not contain type: " + type.toAscii();
		return QSharedPointer<Serializable>();
	}
	if((maxSerializables_ > -1) && (numSourcedSerializables_ >= maxSerializables_))
	{
		error = "Num  created Serializables has exceeded the maximum: " + QString::number(maxSerializables_);
		return QSharedPointer<Serializable>();
	}
	numSourcedSerializables_++;
	if(numSourcedSerializables_ <= defaultSerializables_.size())
		return defaultSerializables_[numSourcedSerializables_-1];
	return generateNewSerializable();
}

QSharedPointer<Serializable> SerializableFactory::generateNewSerializable()
{
	return QSharedPointer<Serializable>((*newSerializableFn_)());
}

//Resets the number of data stores sourced so far
void SerializableFactory::startSourcing()
{
	numSourcedSerializables_ = 0;
	foreach(QSharedPointer<SerializableFactory> factory,factoriesByType_)
	{
		factory->startSourcing();
	}
}

//Returns QSharedPointers to Serializables until all the requirements
//for minimum numbers of Serializables in this factory have been met.
QSharedPointer<Serializable> SerializableFactory::getRequiredSerializable()
{
	QSharedPointer<Serializable> newSerializable;
	foreach(QSharedPointer<SerializableFactory> factory,factoriesByType_)
	{
		newSerializable = factory->getRequiredSerializable();
		if(!newSerializable.isNull())
			return newSerializable;
	}
	if(numSourcedSerializables_ < minSerializables_)
		newSerializable = getSerializable(QString(""));
	return newSerializable;
}

