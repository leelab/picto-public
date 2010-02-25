#include "PictoIdentifiableObject.h"

namespace Picto
{

QMap<QUuid, QSharedPointer<PictoIdentifiableObject> > PictoIdentifiableObject::objects_;

PictoIdentifiableObject::PictoIdentifiableObject()
{
	//Set id_ to null 00000000-0000-0000-0000-000000000000
	id_=QUuid();
	registered_ = false;
}

//! Sets the UUID to the passed in value (used when deserializing XML)
void PictoIdentifiableObject::setId(QUuid id)
{
	if(registered_)
	{
		objects_.remove(id_);
		objects_.insert(id,QSharedPointer<PictoIdentifiableObject>(this));
	}

	id_ = id;
	
}

//! returns the current UUID
QUuid PictoIdentifiableObject::getId()
{
	return id_;
}

//! Generates a new UUID, assigns it to this object, and returns the new UUID
QUuid PictoIdentifiableObject::generateId()
{
	//NOTE: On Windows, this is a GUID, and is pretty much gauranteed to be unique
	//On any other platform, it's probably unique, but might not be.  Given that we
	//aren;t likely to have more than a few hundred (at most!) UUIDs assigned at any 
	//given time, this seems like an acceptable risk.
	QUuid newId = QUuid::createUuid();

	if(registered_)
	{
		objects_.remove(id_);
		objects_.insert(newId,QSharedPointer<PictoIdentifiableObject>(this));
	}

	id_ = newId;
	return id_;
}

//! Registers this object in the list of PictoIdentifiableObjects
void PictoIdentifiableObject::registerObject()
{
	registered_ = true;
	objects_.insert(id_, QSharedPointer<PictoIdentifiableObject>(this));
}

QSharedPointer<PictoIdentifiableObject> PictoIdentifiableObject::getObjectByID(QUuid id)
{
	return objects_.value(id, QSharedPointer<PictoIdentifiableObject>());
}

} //Picto