#ifndef _SIGNAL_DATA_SOURCE_H_
#define _SIGNAL_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSource.h"
#include "SignalDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API SignalDataSource : public AnalysisDataSource
#else
class SignalDataSource : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	SignalDataSource();
	virtual ~SignalDataSource();
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
	QSharedPointer<SignalDataIterator> signalIterator_;
	SignalData lastDataUnit_;
	QString latestValue_;
};
}; //namespace Picto
#endif