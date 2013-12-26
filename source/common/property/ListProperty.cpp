#include <QScriptValue>
#include <QObject>

#include "ListProperty.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new ListProperty with the input name and QVariant(QStringList) value.
*/
ListProperty::ListProperty(QString name, QVariant value):
Property(QVariant::List,name,value)
{
	QStringList val = this->value().toStringList();
	int i=0;
	i++;
}
/*! \brief Implements Property::stringToVariant to convert the input QVariant(QStringList) to a QString with values separated by "::" separaters.
 *	\note To handle the case where a QStringList including a single empty string, we always put a "::" before
 *	and after the list so long as it has a none zero number of entries.
 */
QString ListProperty::variantToString(QVariant value) const
{
	QString returnVal = "";
	QStringList list = value.toStringList();
	//Just in case someone uses a single value list with an empty string, put :: on the front and back of every list
	if(list.size())
		returnVal = "::"+value.toStringList().join("::")+"::";
	return returnVal;
}

/*! \brief Implements Property::stringToVariant() to convert the input "::" separated list string (as returned from variantToString()) to the QVariant(QStringList).  
*/
QVariant ListProperty::stringToVariant(QString string, QString&) const
{
	if(string.isEmpty())
		return QVariant();
	QStringList returnVal = string.split("::");
	returnVal.takeFirst();
	returnVal.takeLast();
	return returnVal;
}

}; //namespace Picto
