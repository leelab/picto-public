#ifndef _ALIGN_TRIGGER_H_
#define _ALIGN_TRIGGER_H_
#include <QString>
#include <QLinkedList>
#include <QSharedPointer>
#include "AnalysisTrigger.h"
#include "AlignDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API AlignTrigger : public AnalysisTrigger
#else
class AlignTrigger : public UIEnabled
#endif
{
	Q_OBJECT
public:
	AlignTrigger();
	virtual ~AlignTrigger();
	static QSharedPointer<Asset> Create();

	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::BEHAVIORAL;};

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif