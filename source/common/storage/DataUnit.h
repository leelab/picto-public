#ifndef _DATAUNIT_H_
#define _DATAUNIT_H_

#include "../common.h"
#include "Serializable.h"

#include <QSharedPointer>
#include <QXmlStreamWriter>

namespace Picto {
#if defined WIN32 || defined WINCE
class PICTOLIB_API DataUnit : public Serializable
#else
class DataUnit : public  Serializable
#endif
{
public:
	DataUnit();

	bool toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	virtual bool serializeDataID(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeDataID(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	qulonglong getDataID();

	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;

private:
	
	static qulonglong generateDataID();
	static qulonglong lastDataID_;
	qulonglong dataID_;
	QString myTagName_;

};

}; //namespace Picto

#endif
