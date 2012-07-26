#ifndef _PERIODSTARTERS_H_
#define _PERIODSTARTERS_H_
#include <QString>
#include <QLinkedList>
#include <QSharedPointer>
#include "AnalysisTrigger.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API StarterContainer : public UIEnabled
#else
class StarterContainer : public UIEnabled
#endif
{
	Q_OBJECT
public:
	StarterContainer();
	virtual ~StarterContainer();
	static QSharedPointer<Asset> Create();

	QList<QSharedPointer<Asset>> getTriggers();
	EventOrderIndex getNextTriggerInList(EventOrderIndex afterIndex);

	virtual QString getUITemplate(){return "StarterContainer";};

protected:

	//Inherited
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif