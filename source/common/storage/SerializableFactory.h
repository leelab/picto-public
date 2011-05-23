#ifndef _SERIALIZABLEFACTORY_H_
#define _SERIALIZABLEFACTORY_H_

#include "Serializable.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>
#include <QMap>

namespace Picto {

#if defined WIN32 || defined WINCE
	class PICTOLIB_API SerializableFactory
#else
class SerializableFactory
#endif
{
public:
	typedef QSharedPointer<Serializable> (*NewSerializableFnPtr)();
	SerializableFactory();
	SerializableFactory(QSharedPointer<Serializable> singleDefaultSerializable);
	SerializableFactory(	int minSerializables,int maxSerializables,NewSerializableFnPtr newSerializableFn = NULL,QVector<QSharedPointer<Serializable>> defaultSerializables = QVector<QSharedPointer<Serializable>>());
	void addSerializableType(QString type,QSharedPointer<SerializableFactory> factory);
	QList<QString> getTypes();

	QSharedPointer<Serializable> getSerializable(QString& error, QString type = "");
	//Resets the number of data stores sourced so far
	void startSourcing();
	//Returns QSharedPointers to Serializables until all the requirements
	//for minimum numbers of Serializables in this factory have been met.
	QSharedPointer<Serializable> getRequiredSerializable();
	int getMaxSerializables(){return maxSerializables_;};
	int getMinSerializables(){return minSerializables_;};
protected:
	virtual QSharedPointer<Serializable> generateNewSerializable();
private:
	QVector<QSharedPointer<Serializable>> defaultSerializables_;
	int minSerializables_;
	int maxSerializables_;
	int numSourcedSerializables_;

	NewSerializableFnPtr newSerializableFn_;
	QMap<QString, QSharedPointer<SerializableFactory>> factoriesByType_;
};

}; //namespace Picto

#endif
