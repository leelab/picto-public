#ifndef _ELEMENT_DATA_SOURCE_H_
#define _ELEMENT_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSource.h"
#include "ElementDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API ElementDataSource : public AnalysisDataSource
#else
class ElementDataSource : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	ElementDataSource();
	virtual ~ElementDataSource();
	static QSharedPointer<Asset> Create();

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif