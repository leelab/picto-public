#ifndef _TIME_DATA_SOURCE_H_
#define _TIME_DATA_SOURCE_H_
#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSourceDep.h"
#include "PropertyDataIterator.h"

namespace Picto {

struct TimeData;
#if defined WIN32 || defined WINCE
class PICTOLIB_API TimeDataSource : public AnalysisDataSourceDep
#else
class TimeDataSource : public UIEnabled
#endif
{
	Q_OBJECT
public:
	TimeDataSource();
	virtual ~TimeDataSource();
	static QSharedPointer<Asset> Create();

	virtual QString getUITemplate(){return "TimeDataSource";};

protected:

	//Inherited
	virtual void fillOutScriptValue(QSharedPointer<AnalysisValue> val);
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};

}; //namespace Picto
#endif