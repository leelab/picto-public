#ifndef _FRAME_TRIGGER_H_
#define _FRAME_TRIGGER_H_
#include <QString>
#include <QLinkedList>
#include <QSharedPointer>
#include "AnalysisTrigger.h"
#include "FrameDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API FrameTrigger : public AnalysisTrigger
#else
class FrameTrigger : public UIEnabled
#endif
{
	Q_OBJECT
public:
	FrameTrigger();
	virtual ~FrameTrigger();
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