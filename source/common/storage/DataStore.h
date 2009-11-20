#ifndef _DATASTORE_H_
#define _DATASTORE_H_

#include "../common.h"

#include <QSharedPointer>
#include <QXmlStreamWriter>

namespace Picto {

class DataStore
{
public:
	DataStore();

	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
};


}; //namespace Picto

#endif
