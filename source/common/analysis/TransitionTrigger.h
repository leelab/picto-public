#ifndef _TRANSITION_TRIGGER_H_
#define _TRANSITION_TRIGGER_H_
#include <QString>
#include <QLinkedList>
#include <QSharedPointer>
#include "AnalysisTrigger.h"
#include "PropertyDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API TransitionTrigger : public AnalysisTrigger
#else
class TransitionTrigger : public AnalysisTrigger
#endif
{
	Q_OBJECT
public:
	TransitionTrigger();
	virtual ~TransitionTrigger();
	static QSharedPointer<Asset> Create();

	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::BEHAVIORAL;};
	virtual QString getUITemplate(){return "TransitionTrigger";};

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif