#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include "../common.h"

#include <QVariant>
#include <QString>
#include <QMap>
#include <QScriptEngine>

namespace Picto {

/*!	\brief Describes the property of an object
 *
 *	Many objects in Picto have properties (e.g. a circle has properties of color 
 *	radius, and location).  The Property object allows us to assign properties to 
 *	objects.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Property
#else
class Property
#endif
{
public:
	Property(int _type, QString _name, QVariant _value);

	int type();
	QString name();
	QMap<QString, QVariant> getAttributes();
	void addAttribute(QString _attributeName, QVariant _attributeValue);
	QVariant value();
	void setValue(QVariant _value);

private:
	int type_;
	QString name_;
	QVariant value_;
	QMap<QString, QVariant> attributes_;
};


}; //namespace Picto

#endif
