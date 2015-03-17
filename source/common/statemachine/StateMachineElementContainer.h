#ifndef _STATE_MACHINE_ELEMENT_CONTAINER_H_
#define _STATE_MACHINE_ELEMENT_CONTAINER_H_

#include <QSharedPointer>
#include <QList>
#include <QScriptEngine>
#include <QScriptEngineDebugger>

#include "../common.h"
#include "../statemachine/UIEnabled.h"
#include "../property/Property.h"
#include "../parameter/AssociateElement.h"
#include "ContainerElement.h"
#include "StateMachineElement.h"

namespace Picto {

/*! \brief A base class for elements containers.
 *
 *	\details This class manages the scoping issues for ContainerElements so that descendant classes can easily
 *	access all in-scope ContainerElements from a single list.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API StateMachineElementContainer : public StateMachineElement
#else
class StateMachineElementContainer : public StateMachineElement
#endif
{
	Q_OBJECT
public:

	/*! \brief Creates a new ElementContainer.
	*/
	StateMachineElementContainer() {};
	virtual ~StateMachineElementContainer() {};

	virtual bool hasEditableDescendants() { return true; };

	void addElements(QSharedPointer<StateMachineElementContainer> elementContainer);
	void addElements(StateMachineElementContainer* elementContainer);

	void addChildElementContainer(QSharedPointer<StateMachineElementContainer> child);
	QList<QSharedPointer<ContainerElement>> getElementList();

	virtual QString friendlyTypeName() { return "State Machine Element Container"; };

	virtual void ClearAssociateChildren(QUuid associateId);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void addElement(QSharedPointer<ContainerElement> element);

	QList<QSharedPointer<ContainerElement> > elements_;
	QList<QSharedPointer<StateMachineElementContainer> > elementContainers_;

protected slots:
	void addChildToElementLists(QSharedPointer<Asset> newChild);

private:
	friend class StateMachineElementContainer;
};

}; //namespace Picto

#endif
