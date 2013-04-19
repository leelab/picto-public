#ifndef _PROPERTY_DATA_SOURCE_H_
#define _PROPERTY_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSourceDep.h"
#include "PropertyDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyDataSource : public AnalysisDataSourceDep
#else
class PropertyDataSource : public AnalysisDataSourceDep
#endif
{
	Q_OBJECT
public:
	PropertyDataSource();
	virtual ~PropertyDataSource();
	static QSharedPointer<Asset> Create();

	virtual QString getUITemplate(){return "PropertyDataSource";};

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif