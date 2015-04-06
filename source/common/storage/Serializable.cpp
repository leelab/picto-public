#include "Serializable.h"
#include "../memleakdetect.h"

using namespace Picto;

QStringList Serializable::errors_;

Serializable::Serializable()
{
}

/*! \brief Returns the latest deserialization errors that were reported from any deserialization event.
 *	\details Since Deserialization of Picto designs is not something that happens simultaneously in multiple threads, we
 *	can just use a static function along with a static list of errors_ to track serialization errors.  The procedure is
 *	to call fromXml() on the node that you want to deserialize then if that function returns false, indicating that
 *	deserialization falied, to call getErrors() to find out what the errors were and clearErrors() to clear out the error
 *	report for next time.
 *	\note In the typical scenario, fromXml() would be called with the entire Picto Design XML document, and getErrors()
 *	would only be used after the entire design had been loaded to report all of the deserialization errors at once.
 */
QString Serializable::getErrors()
{
	QString errorString;

	for(int i=0; i<errors_.length(); i++)
	{
		errorString += errors_[i] + "<br/>";
	}
	return errorString;

}
/*! \brief Converts this Serializable to an XML based QString that can be saved, transferred over the network, etc.
 */
QString Serializable::toXml()
{
	QString returnVal;
	QSharedPointer<QXmlStreamWriter> xmlStreamWriter(new QXmlStreamWriter(&returnVal));
	if(toXml(xmlStreamWriter))
		return returnVal;
	return "";
}
/*! \brief Loads this object from a descriptive xmlText XML QString.
 *	\details Typically the input xmlText would have been created by toXml()
 */
bool Serializable::fromXml(QString xmlText)
{
	QSharedPointer<QXmlStreamReader> xmlStreamReader(new QXmlStreamReader(xmlText));
	// Move read pointer to tag.
	while(!xmlStreamReader->isStartElement() && !xmlStreamReader->atEnd()) 
		xmlStreamReader->readNext();
	Q_ASSERT_X(!xmlStreamReader->atEnd(),"Serializable::fromXml","Couldn't deserialize xmlText: " + xmlText.toLatin1());
	return fromXml(xmlStreamReader);
}

/*! \brief Adds an error message to the static list of errors
 *	\sa getErrors()
 */
void Serializable::addErrorToList(QString errorMsg)
{
	errors_.append(errorMsg);
}