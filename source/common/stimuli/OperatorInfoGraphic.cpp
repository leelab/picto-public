#include <QPainter>

#include "OperatorInfoGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

const QString OperatorInfoGraphic::type = "Operator Info Graphic";

OperatorInfoGraphic::OperatorInfoGraphic()
: VisualElement(QPoint(),QColor(Qt::green))
{
	AddDefinableProperty(QVariant::Rect,"Dimensions",QRect());
	AddDefinableProperty(QVariant::String,"Fields","");
	AddDefinableProperty(QVariant::String,"Values","");
	//These are redefined from Visual Element to fix the default values.
	AddDefinableProperty(QVariant::Bool,"OperatorView",true);
	AddDefinableProperty(QVariant::Bool,"SubjectView",false);
}

void OperatorInfoGraphic::draw()
{
	QRect dimensions = propertyContainer_->getPropertyValue("Dimensions").toRect();
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	p.setFont(QFont("Times",12));
	QString info;
	QStringList fields = propertyContainer_->getPropertyValue("Fields").toString().split(",");
	QStringList values = propertyContainer_->getPropertyValue("Values").toString().split(",");
	QStringList::iterator fIt;
	QStringList::Iterator vIt;
	for (fIt=fields.begin(),vIt=values.begin();
		(fIt != fields.end()) && (vIt != values.end());fIt++,vIt++) 
	{
		info.append(QString("%1: %2, ").arg((*fIt)).arg((*vIt)));
	}
	if(info.size())
		info.chop(2);//Get rid of comma space
	p.drawText(dimensions, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap,info);
	p.end();

	image_ = image;

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* OperatorInfoGraphic::NewVisualElement()
{
	return new OperatorInfoGraphic;
}

QSharedPointer<Asset> OperatorInfoGraphic::Create()
{
	return QSharedPointer<Asset>(new OperatorInfoGraphic());
}

void OperatorInfoGraphic::setData(QString field, QString value)
{
	if(!infoMap_.contains(field))
	{
		orderedFields_.append(field);
	}
	infoMap_[field] = value;
}

void OperatorInfoGraphic::updateValue()
{
	QStringList values;
	foreach(QString field,orderedFields_)
	{
		values.append(infoMap_.value(field));
	}
	propertyContainer_->setPropertyValue("Fields",orderedFields_.join(","));
	propertyContainer_->setPropertyValue("Values",values.join(","));
}

QVariant OperatorInfoGraphic::getData(QString field)
{
	if(infoMap_.contains(field))
		return QVariant(infoMap_[field]);
	return QVariant();
}

void OperatorInfoGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
}

bool OperatorInfoGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
