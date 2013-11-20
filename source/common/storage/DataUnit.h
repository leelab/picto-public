#ifndef _DATAUNIT_H_
#define _DATAUNIT_H_

#include "../common.h"
#include "Serializable.h"

#include <QSharedPointer>
#include <QXmlStreamWriter>

namespace Picto {
/*! \brief A base class for all Picto data types that need to be individually identifiable and communicatable over the network.
 *	\details This class extends the Serializable class to add Data ID functionality, such that descendants of this class
 *	can be serialized over the network, and when they do so will automatically include a Data ID that is unique and ascending
 *	from one resetDataIDGenerator() call to the next.
 *	\note In Picto, resetDataIdGenerator() is called by the Director every time a new Session is started such that Data ID values
 *	are unique over a single session.  Since many behavioral data values are marked by the frame in which they occured, the Data ID
 *	is useful to determine the order in which Behavioral Data Values were generated.  The uniqueness of the ID also lets us assure
 *	that data values that might be sent more than once due to network problems are still only stored once in the session database.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API DataUnit : public Serializable
#else
class DataUnit : public  Serializable
#endif
{
public:
	DataUnit();
	virtual ~DataUnit(){};

	using Serializable::toXml;
	using Serializable::fromXml;
	bool toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	virtual bool serializeDataID(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeDataID(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	qulonglong getDataID();
	/*! \brief Provides a mean to manually set a data ID if the automatically generated one is not sufficient.*/
	void setDataID(qulonglong dataid){dataID_ = dataid;};
	static void resetDataIDGenerator();
	/*! \brief Saves this object to descriptive XML text using the input QXmlStringWriter.
	 *	\details The output XML is meant to be used with a deserializeFromXml() function on the same type of 
	 *	DataUnit.
	 */
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	/*! \brief Loads this object from a QXmlStringReader containing descriptive XML text.
	 *	\details Typically the XML of the input QXmlStreamReader will have been created 
	 *	using serializeAsXml()
	 */
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;

	void addError(QString errorMessage);

private:
	static qulonglong generateDataID();
	static qulonglong lastDataID_;
	qulonglong dataID_;
	QString myTagName_;

};

}; //namespace Picto

#endif
