#include <QScriptValue>
#include <QObject>

#include "MapProperty.h"
#include "../memleakdetect.h"

namespace Picto {

MapProperty::MapProperty(QString name, QVariant value):
Property(QVariant::Map,name,value)
{
}
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
