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
	QSharedPointer<ElementDataIterator> elemIterator_;
	QSharedPointer<AnalysisValue> lastDataUnit_;
	QSharedPointer<AnalysisValue> latestValue_;
};
}; //namespace Picto
#endif