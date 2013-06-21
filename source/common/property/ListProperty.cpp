#include <QScriptValue>
#include <QObject>

#include "ListProperty.h"
#include "../memleakdetect.h"

namespace Picto {

ListProperty::ListProperty(QString name, QVariant value):
Property(QVariant::List,name,value)
{
	QStringList val = this->value().toStringList();
	int i=0;
	i++;
}
QString ListProperty::variantToString(QVariant value) const
{
	QString returnVal = "";
	QStringList list = value.toStringList();
	//Just in case someone uses a single value list with an empty string, put :: on the front and back of every list
	if(list.size())
		returnVal = "::"+value.toStringList().join("::")+"::";
	return returnVal;
}
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
