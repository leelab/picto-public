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

	virtual QString getUITemplate(){return "SignalDataSource";};

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};

}; //namespace Picto
#endif