#ifndef _PROPERTY_TRIGGER_H_
#define _PROPERTY_TRIGGER_H_
#include <QString>
#include <QLinkedList>
#include <QSharedPointer>
#include "AnalysisTrigger.h"
#include "PropertyDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyTrigger : public AnalysisTrigger
#else
class PropertyTrigger : public UIEnabled
#endif
{
	Q_OBJECT
public:
	PropertyTrigger();
	virtual ~PropertyTrigger();
	static QSharedPointer<Asset> Create();

	//Gets the next trigger time following the last one returned.  
	//After restart() is called, it returns the first trigger time in the session.  
	//If there are no more triggers available it returns a negative value.
	virtual EventOrderIndex getNextTriggerTime();

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
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<PropertyDataIterator> propIterator_;
};
}; //namespace Picto
#endif