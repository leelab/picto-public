#ifndef _LFP_TRIGGER_H_
#define _LFP_TRIGGER_H_
#include <QString>
#include <QLinkedList>
#include <QSharedPointer>
#include "AnalysisTrigger.h"
#include "LfpDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API LFPTrigger : public AnalysisTrigger
#else
class LFPTrigger : public UIEnabled
#endif
{
	Q_OBJECT
public:
	LFPTrigger();
	virtual ~LFPTrigger();
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