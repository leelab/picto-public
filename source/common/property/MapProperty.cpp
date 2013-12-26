#include <QScriptValue>
#include <QObject>

#include "MapProperty.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new MapProperty with the input name and QVariant(QVariantMap) value.
*/
MapProperty::MapProperty(QString name, QVariant value):
Property(QVariant::Map,name,value)
{
}

/*! \brief Implements Property::stringToVariant to convert the input QVariant(QVariantMap) to a QString with key value pairs separated by "::" separaters.
 *	\details The format of the returned string is as follows: "key0::value0AsString::key1::value1AsString:: ... keyn::valuenAsString" 
 */
QString MapProperty::variantToString(QVariant value) const
{
	QMap<QString,QVariant> map = value.toMap();
	QString returnVal;
	for(QMap<QString,QVariant>::iterator iter = map.begin();iter != map.end();iter++)
	{
		returnVal.append(iter.key()+"::"+iter.value().toString()+"::");
	}
	if(!returnVal.isEmpty())
		returnVal.chop(2);
	return returnVal;
}
/*! \brief Implements Property::stringToVariant() to convert the input string (in the format returned by variantToString()) into
 *	a QVariant(QVariantMap).
 *	\details If the input string has an invalid format (ie. not every key has a matching value), an error message will be set to
 *	the input error string reference.
 *	\note It looks like even after an uneven number of entries is detected we are continuing through the code and 
 *	the loop that builds the output QVariantMap will then attempt to access an out of bound entry.  This should be looked into.
*/
QVariant MapProperty::stringToVariant(QString string, QString& error) const
{
	QVariantMap returnVal;
	QStringList list = string.split("::");
	//Check if its an empty map
	if(list.size() == 1)
		return QVariant();

	Q_ASSERT(list.size()%2 == 0);	//List must have an even number of elements
	if(list.size()%2 != 0)
		error = "Invalid data entered for map property.  Map string have even number of elements.";
	for(int i=0;i<list.size();i+=2)
	{
		returnVal[list[i]] = list[i+1];
	}
	return returnVal;
}

}; //namespace Picto
