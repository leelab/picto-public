#ifndef _PERIODENDERS_H_
#define _PERIODENDERS_H_
#include <QString>
#include <QLinkedList>
#include <QSharedPointer>
#include "AnalysisTrigger.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API EnderContainer : public UIEnabled
#else
class EnderContainer : public UIEnabled
#endif
{
	Q_OBJECT
public:
	EnderContainer();
	virtual ~EnderContainer();
	static QSharedPointer<Asset> Create();

	QList<QSharedPointer<Asset>> getTriggers();
	EventOrderIndex getNextTriggerInList(EventOrderIndex afterIndex);

	virtual QString getUITemplate(){return "EnderContainer";};

protected:

	//Inherited
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif