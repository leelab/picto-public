#ifndef _SERIALIZABLE_H
#define _SERIALIZABLE_H

#include "../common.h"

#include <QSharedPointer>
#include <QXmlStreamWriter>
#include <QStringList>

namespace Picto {

/*! \brief The top level class of the Picto Design class hierarchy, and the Picto network transferrable data hierarchy, providing an interface for serialization/deserialization
 *	\details Everything element of the Picto design and of network transferrable DataUnit objects must be savable and loadable, so we put that functionality into
 *	this hierarchical root class.  An interface to retrieve serialization errors is also provided.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API Serializable
#else
class Serializable
#endif
{
public:
	Serializable();
	virtual ~Serializable(){};


	static QString getErrors();
	/*! \brief Clears the static list of errors_ to prepare for a new fromXml() call.*/
	static void clearErrors() { errors_.clear(); };
	QString toXml();
	bool fromXml(QString xmlText);
	/*! \brief The same as toXml() except that it takes in a QXmlStreamWriter pointer to which the output XML is written and returns a boolean indicating serialization success.*/
	virtual bool toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	/*! \brief The same as fromXml(QString) except that it takes in a QXMLStreamReader pointer from which it reads the descriptive XML data.  The returned value indicates whether deserialization was successful.*/
	virtual bool fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;

protected:
	void addErrorToList(QString errorMsg);

private:
	
	static QStringList errors_;	//!< A static list of error strings describing deserialization errors.

};

}; //namespace Picto

#endif
