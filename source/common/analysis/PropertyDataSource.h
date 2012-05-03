#ifndef _PROPERTY_DATA_SOURCE_H_
#define _PROPERTY_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSource.h"
#include "PropertyDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyDataSource : public AnalysisDataSource
#else
class PropertyDataSource : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	PropertyDataSource();
	virtual ~PropertyDataSource();
	static QSharedPointer<Asset> Create();

	//AnalysisDataSource specific functions
	virtual void restart();

	//Return the value of this object's property at the input time.
	virtual QSharedPointer<AnalysisValue> getValue(const EventOrderIndex& index);

protected:

	//Inherited
	virtual void recheckSessionData();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<PropertyDataIterator> propIterator_;
	QSharedPointer<PropData> lastDataUnit_;
	QSharedPointer<PropData> latestValue_;
};
}; //namespace Picto
#endif