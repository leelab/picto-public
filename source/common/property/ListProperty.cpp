#include <QScriptValue>
#include <QObject>

#include "ListProperty.h"
#include "../memleakdetect.h"

namespace Picto {

ListProperty::ListProperty(QString name, QVariant value):
Property(QVariant::List,name,value)
{
}
QString ListProperty::variantToString(QVariant value) const
{
	return value.toStringList().join("::");
}
QVariant ListProperty::stringToVariant(QString string, QString&) const
{
	return string.split("::");
}

}; //namespace Picto
