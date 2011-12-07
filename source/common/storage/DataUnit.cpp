#include "DataUnit.h"
#include "../memleakdetect.h"

namespace Picto {

qulonglong DataUnit::lastDataID_ = 0;

DataUnit::DataUnit():
Serializable(),
dataID_(generateDataID())	//Since the data id is used to determine the order in which things occured
							//when they have the same timestamp, it must be generated when the object
							//is generated
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
	xmlStreamWriter->writeTextElement("D",QString::number(dataID_));
	return true;
} 

/*! \brief Deserializes the unique data ID from a DataUnit's XML record if it exists.
 */
bool DataUnit::deserializeDataID(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "D")
	{
		addError("DataUnit", "Unexpected tag", xmlStreamReader);
		return false;
	}
	dataID_ = xmlStreamReader->readElementText().toLongLong();
	return true;
}

//! \brief Returns this DataUnit's dataID
qulonglong DataUnit::getDataID()
{
	return dataID_;
}

void DataUnit::resetDataIDGenerator()
{
	lastDataID_ = 0;
}

qulonglong DataUnit::generateDataID()
{
	return ++lastDataID_;
}

}; //namespace Picto
