#ifndef _DEFAULT_DATA_SOURCE_H_
#define _DEFAULT_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSource.h"
#include "PropertyDataIterator.h"

namespace Picto {

struct TimeData;
/*! \Brief Empty implementation of an AnalysisDataSource used by AnalysisTriggers that don't have any DataSource children.
 *	Since AnalysisTriggers depend on child data sources to tracking the number of triggers in the current analysis period, 
 *	an AnalysisTrigger that has no DataSource children defined should make use of this default DataSource so that it will
 *	be able to accurately set its "length" property.
*/
#if defined WIN32 || defined WINCE
class PICTOLIB_API DefaultDataSource : public AnalysisDataSource
#else
class DefaultDataSource : public UIEnabled
#endif
{
	Q_OBJECT
public:
	DefaultDataSource();
	virtual ~DefaultDataSource();
	static QSharedPointer<Asset> Create();

	virtual void restart();

	//Returns the time of the input index.
	virtual QSharedPointer<AnalysisValue> getValue(const EventOrderIndex& index);

protected:

	//Inherited
	virtual void recheckSessionData();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};

}; //namespace Picto
#endif