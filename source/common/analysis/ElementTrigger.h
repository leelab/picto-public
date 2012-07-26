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
class ElementTrigger : public AnalysisTrigger
#endif
{
	Q_OBJECT
public:
	ElementTrigger();
	virtual ~ElementTrigger();
	static QSharedPointer<Asset> Create();

	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::BEHAVIORAL;};
	virtual QString getUITemplate(){return "ElementTrigger";};

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QStringList boundaryList_;
};
}; //namespace Picto
#endif