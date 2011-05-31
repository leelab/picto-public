#include "DataUnit.h"

namespace Picto {

qulonglong DataUnit::lastDataID_ = 0;

DataUnit::DataUnit():
Serializable(),
dataID_(0)
{
}

bool DataUnit::toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	return serializeAsXml(xmlStreamWriter);
}

bool DataUnit::fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	bool returnVal = deserializeFromXml(xmlStreamReader);
	return returnVal;
}


/*! \brief Used to serialize a unique Data ID into this DataUnit's XML record
 *	This may or may not be called by the child classes serializeAsXml function
 *	depending whether a unique dataID is desired for this DataUnit
 */
bool DataUnit::serializeDataID(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	if(dataID_ == 0)
		dataID_ = generateDataID();
	xmlStreamWriter->writeTextElement("dataid",QString::number(dataID_));
	return true;
} 

/*! \brief Deserializes the unique data ID from a DataUnit's XML record if it exists.
 */
bool DataUnit::deserializeDataID(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "dataid")
	{
		addError("DataUnit", "Unexpected tag", xmlStreamReader);
		return false;
	}
	dataID_ = xmlStreamReader->readElementText().toLongLong();
	return true;
}

void DataUnit::setDataID(qulonglong dataID)
{
	dataID_ = dataID;
}

//! \brief Returns this DataUnit's dataID
qulonglong DataUnit::getDataID()
{
	return dataID_;
}


qulonglong DataUnit::generateDataID()
{
	return ++lastDataID_;
}

}; //namespace Picto
