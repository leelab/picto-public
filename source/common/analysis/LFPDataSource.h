#ifndef _LFP_DATA_SOURCE_H_
#define _LFP_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSourceDep.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API LFPDataSource : public AnalysisDataSourceDep
#else
class LFPDataSource : public AnalysisDataSourceDep
#endif
{
	Q_OBJECT
public:
	LFPDataSource();
	virtual ~LFPDataSource();
	static QSharedPointer<Asset> Create();

	virtual QString getUITemplate(){return "LFPDataSource";};
protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif