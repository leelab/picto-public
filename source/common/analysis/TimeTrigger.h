#ifndef _TIME_TRIGGER_H_
#define _TIME_TRIGGER_H_
#include <QString>
#include <QLinkedList>
#include <QSharedPointer>
#include "AnalysisTrigger.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API TimeTrigger : public AnalysisTrigger
#else
class TimeTrigger : public UIEnabled
#endif
{
	Q_OBJECT
public:
	TimeTrigger();
	virtual ~TimeTrigger();
	static QSharedPointer<Asset> Create();

	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::OTHER;};

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif