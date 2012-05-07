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
	//After this function is called, the first trigger in the session should 
	//be returned from getNextTriggerTime()
	virtual void restart();

	//Returns the fraction of triggers that still need to
	//be processed for this session (that this object knows about).
	virtual float fractionTriggersRemaining();

	QSharedPointer<AnalysisValue> getLatestValue();

protected:

	//Inherited
	//Gets the next trigger time following the last one returned.  
	//After restart() is called, it returns the first trigger time in the session.  
	//If there are no more triggers available it returns a negative value.
	virtual EventOrderIndex getNextTriggerTime();
	virtual void recheckSessionData();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<LFPDataIterator> lfpIterator_;
	QSharedPointer<AnalysisValue> latestValue_;
};
}; //namespace Picto
#endif