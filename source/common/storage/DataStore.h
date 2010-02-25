/*! \brief a base class for anything that needs to write itself out as XML
 *
 *	Since most stuff is stored as XML, this class is incredibly useful (and yet really 
 *	simple).  Any object that is going to store itself as XML needs to inheret from
 *	this class and implement the two virtual functions for serializing/deserializing
 *	itself.
 *
 *	The DataStore class actually has no real functionality (it's basically just an interface).
 *
 *	The object is responsible for serializing/desersialing a complete XML fragment.  For example
 *	a VisualElement object will need to generate the tags <VisualElement> </VisualElement>, and
 *	all of the content in between.  Likewise, when deserialize is called, the VisualElement 
 *	should expect to receive an XMLStreamWriter that has <VisualElement> as the current token, and
 *	then will return with the </VisualElement> tag as the current token.
 *	
 */

#ifndef _DATASTORE_H_
#define _DATASTORE_H_

#include "../common.h"

#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <QRect>
#include <QPoint>
#include <QColor>

namespace Picto {

class DataStore
{
public:
	DataStore();

	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;

protected:
	//To make the serialization and desrialization routines easier, the following mini
	//functions are provided, for serializing and deserializing common data types.
	//Note that these don't cover all possible data types, so you should expect
	//to write some of your own code...
	void serializeQPoint(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
					QString name, QPoint point);
	void serializeQRect(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
					QString name, QRect rect);
	void serializeQColor(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
					QString name, QColor color);

	QPoint deserializeQPoint(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QRect deserializeQRect(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QColor deserializeQColor(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
