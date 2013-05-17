#ifndef _UIElement_H_
#define _UIElement_H_
#include <QUuid>

#include "../common.h"
#include "../storage/DataStore.h"
#include "../parameter/AssociateElement.h"

namespace Picto {

/*!	\brief A UIElement is the exit point from a StateMachineElement or ControlElement
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API UIElement : public DataStore, public AssociateElement
#else
class UIElement : public DataStore, public AssociateElement
#endif
{
	Q_OBJECT
public:
	UIElement();
	virtual ~UIElement(){};

	static QSharedPointer<Asset> Create();

	void setPos(QPoint pos);
	QPoint getPos();
	void setNotes(QString notes);
	QString getNotes();
	virtual QString friendlyTypeName(){return "UI Element";};

	ASSOCIATE_ELEMENT_IMPLEMENTATION

protected:
	virtual QString defaultTagName(){return "UIElement";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
