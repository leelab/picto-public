/*!	\brief A StateMachineElement is a piece that can be used in a state machine
 *	
 *	StateMachineElements would include FlowElements, States, Trials, 
 *	Stages, and States.  Not all StateMachineElements need all of
 *	the functionality provided.
 */
/*!	A base class for all objects that can be tagged with a UUID
 *
 *	Within the Picto system, there are objects that are unique (Scenes, 
 *	states, parameters, etc).  To make it easy to find these items, the
 *	PictoIdentifiableObject base class is used.  It serves multiple functions:
 *		1. Allows us to tag objects with globally unique IDs
 *		2. Allows us to recover objects by their ID
 *	Note that any PictoIdentifiableObject is effectively a "global variable"
 *	in the sense that it can be recovered from anywhere in the system
 *
 */


#ifndef _PICTO_IDENTIFIABLE_OBJECT_H_
#define _PICTO_IDENTIFIABLE_OBJECT_H_

#include <QUuid>
#include <QMap>
#include <QSharedPointer>

#include "../common.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PictoIdentifiableObject
#else
class PictoIdentifiableObject
#endif
{
public:
	PictoIdentifiableObject();
	PictoIdentifiableObject(QUuid id);

	void setId(QUuid id);
	QUuid getId();
	QUuid generateId();

	void registerObject();

	static QSharedPointer<PictoIdentifiableObject> getObjectByID(QUuid id);

protected:
	QUuid id_;
	bool registered_;

	static QMap<QUuid, QSharedPointer<PictoIdentifiableObject> > objects_;
};


}; //namespace Picto

#endif
