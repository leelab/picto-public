#ifndef _SPIKE_TRIGGER_H_
#define _SPIKE_TRIGGER_H_
#include <QString>
#include <QLinkedList>
#include <QSharedPointer>
#include "AnalysisTrigger.h"
#include "SpikeDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API SpikeTrigger : public AnalysisTrigger
#else
class SpikeTrigger : public UIEnabled
#endif
{
	Q_OBJECT
public:
	SpikeTrigger();
	virtual ~SpikeTrigger();
	static QSharedPointer<Asset> Create();

	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::NEURAL;};

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif