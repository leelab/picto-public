#ifndef _SPIKE_DATA_SOURCE_H_
#define _SPIKE_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSourceDep.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API SpikeDataSource : public AnalysisDataSourceDep
#else
class SpikeDataSource : public AnalysisDataSourceDep
#endif
{
	Q_OBJECT
public:
	SpikeDataSource();
	virtual ~SpikeDataSource();
	static QSharedPointer<Asset> Create();

	virtual QString getUITemplate(){return "SpikeDataSource";};

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};
}; //namespace Picto
#endif