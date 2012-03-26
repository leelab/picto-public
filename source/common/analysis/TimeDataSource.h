#ifndef _TIME_DATA_SOURCE_H_
#define _TIME_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSource.h"
#include "PropertyDataIterator.h"

namespace Picto {

struct TimeData;
#if defined WIN32 || defined WINCE
class PICTOLIB_API TimeDataSource : public AnalysisDataSource
#else
class TimeDataSource : public UIEnabled
#endif
{
	Q_OBJECT
public:
	TimeDataSource();
	virtual ~TimeDataSource();
	static QSharedPointer<Asset> Create();

	virtual void restart();

	//Returns the time of the input index.
	virtual QSharedPointer<AnalysisValue> getValue(const EventOrderIndex& index);

public slots:
	double value(int triggerIndex);

protected:

	//Inherited
	virtual void recheckSessionData();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
struct TimeData : public AnalysisValue
{
	TimeData(double t){time=t;};
	double time;
};

}; //namespace Picto
#endif