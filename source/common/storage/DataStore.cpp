#include <QVariant>

#include "DataStore.h"

namespace Picto {

QStringList DataStore::errors_;

DataStore::DataStore()
{
}

/*! \brief Adds an error message to the list of errors
 *
 *	We maintain a list of erros so that errors can be tracked through the deserialization
 *	process.
 */
void DataStore::addError(QString objectType, QString errorMsg, QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QString newErr = "ERROR\n";
	newErr += "Object: " + objectType + "\n";
	newErr += QString("Line: %1\n").arg(xmlStreamReader->lineNumber());
	newErr += "Name: " + xmlStreamReader->name().toString() + "\n";
	newErr += "Element: " + xmlStreamReader->tokenString() + "\n";
	newErr += "Message: " + errorMsg + "\n";

	errors_.append(newErr);
}

/*!	\brief Returns a string listing all errors that have occured
 */
QString DataStore::getErrors()
{
	QString errorString = "DESERIALIZATION ERRORS\n\n";

	for(int i=0; i<errors_.length(); i++)
	{
		errorString += errors_[i] + "\n";
	}
	return errorString;

}



void DataStore::serializeQPoint(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
						QString name, QPoint point)
{
	Q_ASSERT(!name.isEmpty());
	xmlStreamWriter->writeStartElement(name);
	xmlStreamWriter->writeAttribute("datatype", "QPoint");
	xmlStreamWriter->writeAttribute("x",QString("%1").arg(point.x()));
	xmlStreamWriter->writeAttribute("y",QString("%1").arg(point.y()));
	xmlStreamWriter->writeEndElement();

}

void DataStore::serializeQRect(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
						QString name, QRect rect)
{
	Q_ASSERT(!name.isEmpty());
	xmlStreamWriter->writeStartElement(name);
	xmlStreamWriter->writeAttribute("datatype", "QRect");
	xmlStreamWriter->writeAttribute("x",QString("%1").arg(rect.x()));
	xmlStreamWriter->writeAttribute("y",QString("%1").arg(rect.y()));
	xmlStreamWriter->writeAttribute("width",QString("%1").arg(rect.width()));
	xmlStreamWriter->writeAttribute("height",QString("%1").arg(rect.height()));
	xmlStreamWriter->writeEndElement();

}

void DataStore::serializeQColor(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
					QString name, QColor color)
{
	Q_ASSERT(!name.isEmpty());
	xmlStreamWriter->writeStartElement(name);
	xmlStreamWriter->writeAttribute("datatype", "QColor");
	xmlStreamWriter->writeAttribute("R",QString("%1").arg(color.red()));
	xmlStreamWriter->writeAttribute("G",QString("%1").arg(color.green()));
	xmlStreamWriter->writeAttribute("B",QString("%1").arg(color.blue()));
	xmlStreamWriter->writeAttribute("A",QString("%1").arg(color.alpha()));
	xmlStreamWriter->writeEndElement();

}


QPoint DataStore::deserializeQPoint(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QPoint point;
	point.setX(xmlStreamReader->attributes().value("x").toString().toInt());
	point.setY(xmlStreamReader->attributes().value("y").toString().toInt());
	return point;
}

QRect DataStore::deserializeQRect(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QRect rect;
	rect.setX(xmlStreamReader->attributes().value("x").toString().toInt());
	rect.setY(xmlStreamReader->attributes().value("y").toString().toInt());
	rect.setWidth(xmlStreamReader->attributes().value("width").toString().toInt());
	rect.setHeight(xmlStreamReader->attributes().value("height").toString().toInt());
	return rect;
}

QColor DataStore::deserializeQColor(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QColor color;
	color.setRed(xmlStreamReader->attributes().value("R").toString().toInt());
	color.setGreen(xmlStreamReader->attributes().value("G").toString().toInt());
	color.setBlue(xmlStreamReader->attributes().value("B").toString().toInt());
	color.setAlpha(xmlStreamReader->attributes().value("A").toString().toInt());
	return color;
}

}; //namespace Picto
