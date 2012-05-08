#ifndef _LFP_DATA_SOURCE_H_
#define _LFP_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSource.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API LFPDataSource : public AnalysisDataSource
#else
class LFPDataSource : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	LFPDataSource();
	virtual ~LFPDataSource();
	static QSharedPointer<Asset> Create();

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif