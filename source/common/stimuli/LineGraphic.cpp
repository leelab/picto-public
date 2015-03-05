#include <QPainter>
#include "LineGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

	const QString LineGraphic::type = "Line Graphic";

LineGraphic::LineGraphic(QPoint position, QVector<QPoint> points, QColor color)
: VisualElement(position,color)
{
	AddDefinableProperty("Points","(1,1)(2,2)...(n,n)");
}

void LineGraphic::draw()
{
	int left = 0, top = 0, right = 0, bottom = 0;

	foreach(QPoint point, points_)
	{
		if(point.x() < left) left = point.x();
		if(point.x() > right) right = point.x();
		if(point.y() < top) top = point.y();
		if(point.y() > bottom) bottom = point.y();
	}

	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(right-left,bottom-top,QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	for(int i = 1; i < points_.count(); i++)
	{
		p.drawLine(points_[i-1],points_[i]);
	}
	p.end();
	image_ = image;

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

/*! \brief Creates a new LineGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> LineGraphic::Create()
{
	return QSharedPointer<Asset>(new LineGraphic());
}

void LineGraphic::postDeserialize()
{
	VisualElement::postDeserialize();

	QString pointsStr = propertyContainer_->getPropertyValue("Points").toString();
	QStringList pointsSep = pointsStr.split(")",QString::SkipEmptyParts);
	bool xOk = true, yOk = true;
	foreach(QString pointStr,pointsSep)
	{
		pointStr.replace(" ","");
		pointStr.replace("(","");
		QStringList xAndY = pointStr.split(",");
		if(xAndY.size() != 2)
			return;
		points_.push_back(QPoint(xAndY.first().toInt(&xOk),xAndY.last().toInt(&yOk)));
		if(!(xOk && yOk))
			return;
	}
	draw();
}

bool LineGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;

	QString pointsStr = propertyContainer_->getPropertyValue("Points").toString();
	QStringList pointsSep = pointsStr.split(")",QString::SkipEmptyParts);
	bool xOk = true, yOk = true;
	foreach(QString pointStr,pointsSep)
	{
		pointStr.replace(" ","");
		pointStr.replace("(","");
		QStringList xAndY = pointStr.split(",");
		if(xAndY.size() != 2)
		{
			addError("Points string is incorrectly formed: " + pointsStr.toLatin1() + ".  Should be: (1,1)(2,2)...(n,n).");
			return false;
		}
		xAndY.first().toInt(&xOk);
		xAndY.last().toInt(&yOk);
		if(!(xOk && yOk))
		{
			addError("Points string is incorrectly formed: " + pointsStr.toLatin1() + ".  Should be: (1,1)(2,2)...(n,n).");
			return false;
		}
	}
	return true;
}

}; //namespace Picto
