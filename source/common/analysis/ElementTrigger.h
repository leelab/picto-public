#ifndef _ELEMENT_TRIGGER_H_
#define _ELEMENT_TRIGGER_H_
#include <QString>
#include <QLinkedList>
#include <QSharedPointer>
#include "AnalysisTrigger.h"
#include "TransitionDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API ElementTrigger : public AnalysisTrigger
#else
class ElementTrigger : public UIEnabled
#endif
{
	Q_OBJECT
public:
	ElementTrigger();
	virtual ~ElementTrigger();
	static QSharedPointer<Asset> Create();

	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::BEHAVIORAL;};
	//After this function is called, the first trigger in the session should 
	//be returned from getNextTriggerTime()
	virtual void restart();

	//Returns the total number of triggers in this session
	//that this object knows about.
	virtual unsigned int totalKnownTriggers();
	//Returns the number of triggers that still need to
	//be processed for this session (that this object knows about).
	virtual unsigned int remainingKnownTriggers();

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
	QSharedPointer<TransitionDataIterator> transIterator_;
	QStringList boundaryList_;
};
}; //namespace Picto
#endif