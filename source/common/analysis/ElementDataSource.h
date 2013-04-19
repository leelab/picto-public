#ifndef _ELEMENT_DATA_SOURCE_H_
#define _ELEMENT_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSourceDep.h"
#include "ElementDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API ElementDataSource : public AnalysisDataSourceDep
#else
class ElementDataSource : public AnalysisDataSourceDep
#endif
{
	Q_OBJECT
public:
	ElementDataSource();
	virtual ~ElementDataSource();
	static QSharedPointer<Asset> Create();
	virtual QString getUITemplate(){return "ElementDataSource";};

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif