#ifndef _SPIKE_DATA_SOURCE_H_
#define _SPIKE_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSource.h"
#include "PropertyDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API SpikeDataSource : public AnalysisDataSource
#else
class SpikeDataSource : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	SpikeDataSource();
	virtual ~SpikeDataSource();
	static QSharedPointer<Asset> Create();

	//AnalysisDataSource specific functions
	virtual void restart();

	//Return the value of this object's property at the input time.
	virtual QString getValue(const EventOrderIndex& index);

protected:

	//Inherited
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<PropertyDataIterator> propIterator_;
	PropData lastDataUnit_;
	QString latestValue_;
};
}; //namespace Picto
#endif