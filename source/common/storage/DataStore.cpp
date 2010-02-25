#include <QVariant>

#include "DataStore.h"

namespace Picto {

DataStore::DataStore()
{
}


void DataStore::serializeQPoint(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
						QString name, QPoint point)
{
	xmlStreamWriter->writeStartElement("QPoint");
	if(!name.isEmpty())
		xmlStreamWriter->writeAttribute("name",name);
	xmlStreamWriter->writeAttribute("x",QString("%1").arg(point.x()));
	xmlStreamWriter->writeAttribute("y",QString("%1").arg(point.y()));
	xmlStreamWriter->writeEndElement();

}

void DataStore::serializeQRect(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
						QString name, QRect rect)
{
	xmlStreamWriter->writeStartElement("QRect");
	if(!name.isEmpty())
		xmlStreamWriter->writeAttribute("name",name);
	xmlStreamWriter->writeAttribute("x",QString("%1").arg(rect.x()));
	xmlStreamWriter->writeAttribute("y",QString("%1").arg(rect.y()));
	xmlStreamWriter->writeAttribute("width",QString("%1").arg(rect.width()));
	xmlStreamWriter->writeAttribute("height",QString("%1").arg(rect.height()));
	xmlStreamWriter->writeEndElement();

}

void DataStore::serializeQColor(QSharedPointer<QXmlStreamWriter> xmlStreamWriter, 
					QString name, QColor color)
{
	xmlStreamWriter->writeStartElement("QColor");
	if(!name.isEmpty())
		xmlStreamWriter->writeAttribute("name",name);
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
