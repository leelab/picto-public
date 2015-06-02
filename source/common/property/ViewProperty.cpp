#include <QScriptValue>
#include <QObject>

#include "ViewProperty.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new ViewProperty with the input name and QVariant(ViewProperty) value.
 */
ViewProperty::ViewProperty(QString name, QVariant value)
	: Property( typeId(), name, value)
{
}

QString ViewProperty::variantToString(QVariant value) const
{
	ViewProperties props = value.value<ViewProperties>();
	return QString("%1,%2,%3").arg((int)props.size_).arg(props.x_).arg(props.y_);
}

QVariant ViewProperty::stringToVariant(QString string, QString& error) const
{
	error = "";
	QStringList values = string.split(",");
	if (values.size() != 3)
	{
		error = "Invalid data entered for ViewProperty";
		return QVariant::fromValue(ViewProperties());
	}
	ViewProperties props((DataViewSize::ViewSize)values[0].toInt(), values[1].toInt(), values[2].toInt());
	return QVariant::fromValue(props);
}

}; //namespace Picto
