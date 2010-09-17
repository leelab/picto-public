#ifndef _PICTO_IDENTIFIABLE_OBJECT_H_
#define _PICTO_IDENTIFIABLE_OBJECT_H_

#include <QUuid>
#include <QMap>
#include <QSharedPointer>

#include "../common.h"

namespace Picto {

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
 *	Although this was implemented, it was never used (I ended up finding an
 *	alternate solution).  However, the code is still useful, so I have
 *	left it in place.
 *
 */

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

private:
	QUuid id_;
	bool registered_;

	static QMap<QUuid, QSharedPointer<PictoIdentifiableObject> > objects_;
};


}; //namespace Picto

#endif
