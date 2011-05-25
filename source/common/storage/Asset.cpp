#include "Asset.h"

namespace Picto {

Asset::Asset():
Serializable(),
QObject(NULL),
isNew_(true),	//!!!!!!!!THIS NEEDS TO BE INITIALIZED TO TRUE.  I JUST CHANGED IT FOR DEBUGGING PURPOSES!!!!!!!!!!!!!
edited_(false),
deleted_(false)
{
	connect(this, SIGNAL(edited()), this, SLOT(receivedEditedSignal()));
	connect(this, SIGNAL(deleted()), this, SLOT(receivedDeletedSignal()));
}

bool Asset::toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	if(isDeleted())
		return true;
	return serializeAsXml(xmlStreamWriter);
}

bool Asset::fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	bool returnVal = deserializeFromXml(xmlStreamReader);
	edited_ = false;
	isNew_ = false;
	deleted_ = false;
	return returnVal;
}
void Asset::setDeleted()
{
	emit deleted();
}

void Asset::receivedEditedSignal()
{
	edited_ = true;
}
void Asset::receivedDeletedSignal()
{
	deleted_ = true;
}

}; //namespace Picto
