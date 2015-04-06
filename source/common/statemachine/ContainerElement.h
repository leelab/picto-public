#ifndef _CONTAINER_ELEMENT_H_
#define _CONTAINER_ELEMENT_H_
#include <QSharedPointer>
#include <QString>

#include "../common.h"

#include "../statemachine/scriptablecontainer.h"

namespace Picto {

/*!	\brief A Base class for all Elements representing data that is to be handled by a StateMachineElementContainer.
 *
 *	\details It is useful to be able to group together these types of elements such that they can be handled in similar
 *	ways.  This way, any element that needs to be handled by an ElementContainer can inherit ContainerElement and all of
 *	its associated logic.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_CLASS ContainerElement : public ScriptableContainer
#else
	class ContainerElement : public ScriptableContainer
#endif
{
	Q_OBJECT
public:
	ContainerElement(){};
	virtual ~ContainerElement(){};

	virtual QString friendlyTypeName(){return "Container Element";};
	virtual QString getUIGroup(){return "None";};
};

}; //namespace Picto

#endif
