#ifndef _ALIGN_DATA_SOURCE_H_
#define _ALIGN_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSource.h"
#include "AlignDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API AlignDataSource : public AnalysisDataSource
#else
class AlignDataSource : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	AlignDataSource();
	virtual ~AlignDataSource();
	static QSharedPointer<Asset> Create();

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif