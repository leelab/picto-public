#ifndef _SIGNAL_TRIGGER_H_
#define _SIGNAL_TRIGGER_H_
#include <QString>
#include <QLinkedList>
#include <QSharedPointer>
#include "AnalysisTrigger.h"
#include "SignalDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API SignalTrigger : public AnalysisTrigger
#else
class SignalTrigger : public AnalysisTrigger
#endif
{
	Q_OBJECT
public:
	SignalTrigger();
	virtual ~SignalTrigger();
	static QSharedPointer<Asset> Create();

	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::BEHAVIORAL;};
	virtual QString getUITemplate(){return "SignalTrigger";};

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif