#include <QPainter>
#include "TokenFactoryGraphic.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

#define PI 3.141592
const QString TokenFactoryGraphic::type = "Token Factory Graphic";

TokenFactoryGraphic::TokenFactoryGraphic(QPoint position, QRect dimensions, QColor color)
:
VisualElement(position,color)
{
	//updatingParameterLists_ = false;
	shapeList_ << "Ellipse" << "Rectangle" << "Diamond";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"TokenShape",0,"enumNames",shapeList_);
	AddDefinableProperty(QVariant::Size,"TokenSize",dimensions.size());

	AddDefinableProperty(QVariant::Int,"NumTokens",0);

	AddDefinableProperty(QVariant::Bool,"Outline",false);
	AddDefinableProperty(QVariant::Int,"OutlineWidth",0);

	//These are not be visible to the user in the designer window.
	listNames_.push_back("TokenSizes");
	listNames_.push_back("TokenColors");
	listNames_.push_back("TokenShapes");
	listNames_.push_back("TokenOutlines");
	listNames_.push_back("TokenOutlineWidths");
	listNames_.push_back("TokenXs");
	listNames_.push_back("TokenYs");
	foreach(QString listName,listNames_)
	{
		AddDefinableProperty(QVariant::String,listName,"");
	}

	deltaHashs_.push_back(&sizeDelta_);
	deltaHashs_.push_back(&colorDelta_);
	deltaHashs_.push_back(&shapeDelta_);
	deltaHashs_.push_back(&outlineDelta_);
	deltaHashs_.push_back(&outlineWidthDelta_);
	deltaHashs_.push_back(&xDelta_);
	deltaHashs_.push_back(&yDelta_);
}
QRect TokenFactoryGraphic::getTokenDimensions()
{
	return QRect(QPoint(),propertyContainer_->getPropertyValue("TokenSize").toSize());
}

void TokenFactoryGraphic::setTokenDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("TokenSize",dimensions.size());
}

QString TokenFactoryGraphic::getTokenShape()
{
	return shapeList_.value(propertyContainer_->getPropertyValue("TokenShape").toInt(),"");
}

void TokenFactoryGraphic::setTokenShape(QString shape)
{
	if(shapeList_.contains(shape))
		propertyContainer_->setPropertyValue("TokenShape",shapeList_.indexOf(shape));
}

void TokenFactoryGraphic::setNumTokens(int num)
{
	if(num < 0)
		num = 0;
	propertyContainer_->setPropertyValue("NumTokens",num);
	updateListSizes();
}
int TokenFactoryGraphic::getNumTokens()
{
	return propertyContainer_->getPropertyValue("NumTokens").toInt();
}

bool TokenFactoryGraphic::getOutline()
{
	return propertyContainer_->getPropertyValue("Outline").toBool();
}

void TokenFactoryGraphic::setOutline(bool on)
{
	propertyContainer_->setPropertyValue("Outline",on);
}

int TokenFactoryGraphic::getOutlineWidth()
{
	return propertyContainer_->getPropertyValue("OutlineWidth").toInt();
}

void TokenFactoryGraphic::setOutlineWidth(int pixels)
{
	if(pixels < 0)
		pixels = 0;
	propertyContainer_->setPropertyValue("OutlineWidth",pixels);
}

QPoint TokenFactoryGraphic::getPositionOffset()
{
	return posOffset_;
}

void TokenFactoryGraphic::setTokenPos(int index,int x,int y)
{
	setTokenX(index,x);
	setTokenY(index,y);
}

void TokenFactoryGraphic::setTokenLocalPos(int index,int x,int y)
{
	setTokenLocalX(index,x);
	setTokenLocalY(index,y);
}

void TokenFactoryGraphic::setTokenX(int index,int x)
{
	int offset = getPosition().x();
	setTokenLocalX(index,x-offset);
}

void TokenFactoryGraphic::setTokenY(int index,int y)
{
	int offset = getPosition().y();
	setTokenLocalY(index,y-offset);}

void TokenFactoryGraphic::setTokenLocalX(int index,int x)
{
	if(index < 0 || index >= getNumTokens())
		return;
	QStringList xs = propertyContainer_->getPropertyValue("TokenXs").toString().split(",",QString::SkipEmptyParts);
	xs[index] = QString::number(x);
	propertyContainer_->setPropertyValue("TokenXs",xs.join(","));
}

void TokenFactoryGraphic::setTokenLocalY(int index,int y)
{
	if(index < 0 || index >= getNumTokens())
		return;
	QStringList ys = propertyContainer_->getPropertyValue("TokenYs").toString().split(",",QString::SkipEmptyParts);
	ys[index] = QString::number(y);
	propertyContainer_->setPropertyValue("TokenYs",ys.join(","));
}

void TokenFactoryGraphic::setTokenSize(int index,double size)
{
	if(size > 1.0)
		size = 1.0;
	if(size < 0)
		size = 0.0;
	if(index < 0 || index >= getNumTokens())
		return;
	QStringList sizes = propertyContainer_->getPropertyValue("TokenSizes").toString().split(",",QString::SkipEmptyParts);
	sizes[index] = QString::number(size);
	propertyContainer_->setPropertyValue("TokenSizes",sizes.join(","));
}

void TokenFactoryGraphic::setTokenColor(int index, int r, int g, int b, int a)
{
	if(index < 0 || index >= getNumTokens())
		return;
	QStringList colors = propertyContainer_->getPropertyValue("TokenColors").toString().split(",",QString::SkipEmptyParts);
	colors[index] = QColor(r,g,b,a).name();
	propertyContainer_->setPropertyValue("TokenColors",colors.join(","));
}

void TokenFactoryGraphic::setTokenColor(int index,QVariant color)
{
	QColor col = color.value<QColor>();
	setTokenColor(index,col.red(),col.green(),col.blue());
}

void TokenFactoryGraphic::setTokenShape(int index, QString shape)
{
	if(index < 0 || index >= getNumTokens())
		return;
	if(!shapeList_.contains(shape))
		return;
	QStringList shapes = propertyContainer_->getPropertyValue("TokenShapes").toString().split(",",QString::SkipEmptyParts);
	shapes[index] = shape;
	propertyContainer_->setPropertyValue("TokenShapes",shapes.join(","));
}
void TokenFactoryGraphic::setTokenOutline(int index, bool on)
{
	if(index < 0 || index >= getNumTokens())
		return;
	QStringList outlines = propertyContainer_->getPropertyValue("TokenOutlines").toString().split(",",QString::SkipEmptyParts);
	outlines[index] = QString::number(int(on));
	propertyContainer_->setPropertyValue("TokenOutlines",outlines.join(","));
}
void TokenFactoryGraphic::setTokenOutlineWidth(int index, int pixels)
{
	if(index < 0 || index >= getNumTokens())
		return;
	QStringList outlineWidths = propertyContainer_->getPropertyValue("TokenOutlineWidths").toString().split(",",QString::SkipEmptyParts);
	outlineWidths[index] = QString::number(pixels);
	propertyContainer_->setPropertyValue("TokenOutlineWidths",outlineWidths.join(","));
}

int TokenFactoryGraphic::getTokenX(int index)
{
	return getPosition().x() + getTokenLocalX(index);
}

int TokenFactoryGraphic::getTokenY(int index)
{
	return getPosition().y() + getTokenLocalY(index);
}

int TokenFactoryGraphic::getTokenLocalX(int index)
{
	QStringList xs = propertyContainer_->getPropertyValue("TokenXs").toString().split(",",QString::SkipEmptyParts);
	if(index < 0 || index >= xs.size())
		return 0;
	QString val = xs[index];
	if(val == "_")
		return 0;
	return val.toInt();
}

int TokenFactoryGraphic::getTokenLocalY(int index)
{
	QStringList ys = propertyContainer_->getPropertyValue("TokenYs").toString().split(",",QString::SkipEmptyParts);
	if(index < 0 || index >= ys.size())
		return 0;
	QString val = ys[index];
	if(val == "_")
		return 0;
	return val.toInt();
}
double TokenFactoryGraphic::getTokenSize(int index)
{
	if(index >= getNumTokens() || index < 0)
		return -1;
	QStringList sizes = propertyContainer_->getPropertyValue("TokenSizes").toString().split(",",QString::SkipEmptyParts);
	QString result = "_";
	if(sizes.size()>=index)
		result = sizes[index];
	if(result == "_")
		return 1.0;
	return result.toDouble();
}
int TokenFactoryGraphic::getTokenRed(int index)
{
	return getTokenColor(index).red();
}
int TokenFactoryGraphic::getTokenGreen(int index)
{
	return getTokenColor(index).green();
}
int TokenFactoryGraphic::getTokenBlue(int index)
{
	return getTokenColor(index).blue();
}
int TokenFactoryGraphic::getTokenAlpha(int index)
{
	return getTokenColor(index).alpha();
}
QColor TokenFactoryGraphic::getTokenColor(int index)
{
	if(index > getNumTokens() || index < 0)
		return QColor();
	QStringList colors = propertyContainer_->getPropertyValue("TokenColors").toString().split(",",QString::SkipEmptyParts);
	
	QString result = "_";
	if(colors.size()>=index)
		result = colors[index];
	if(result == "_")
		return getColor();;
	return QColor(result);
}
bool TokenFactoryGraphic::getTokenOutline(int index)
{
	if(index >= getNumTokens() || index < 0)
		return false;
	QStringList outlines = propertyContainer_->getPropertyValue("TokenOutlines").toString().split(",",QString::SkipEmptyParts);
	QString result = "_";
	if(outlines.size()>=index)
		result = outlines[index];
	if(result == "_")
		return getOutline();
	return result.toInt();
}
int TokenFactoryGraphic::getTokenOutlineWidth(int index)
{
	if(index >= getNumTokens() || index < 0)
		return -1;
	QStringList outlineWidths = propertyContainer_->getPropertyValue("TokenOutlineWidths").toString().split(",",QString::SkipEmptyParts);
	
	QString result = "_";
	if(outlineWidths.size()>=index)
		result = outlineWidths[index];
	if(result == "_")
		return getOutlineWidth();
	return result.toInt();
}
QString TokenFactoryGraphic::getTokenShape(int index)
{
	if(index > getNumTokens() || index < 0)
		return "";
	QStringList shapes = propertyContainer_->getPropertyValue("TokenShapes").toString().split(",",QString::SkipEmptyParts);
	QString result = "_";
	if(shapes.size()>=index)
		result = shapes[index];
	if(result == "_")
		return getTokenShape();
	return result;
}

void TokenFactoryGraphic::draw()
{
	int numTokens = getNumTokens();
	QRect trayDimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
	QRect tokenDimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("TokenSize").toSize());
	//Make sure size of sizes,colors,shapes lists matches numTokens before drawing
	QStringList sizes = propertyContainer_->getPropertyValue("TokenSizes").toString().split(",",QString::SkipEmptyParts);
	QStringList colors = propertyContainer_->getPropertyValue("TokenColors").toString().split(",",QString::SkipEmptyParts);
	QStringList shapes = propertyContainer_->getPropertyValue("TokenShapes").toString().split(",",QString::SkipEmptyParts);
	QStringList outlines = propertyContainer_->getPropertyValue("TokenOutlines").toString().split(",",QString::SkipEmptyParts);
	QStringList outlineWidths = propertyContainer_->getPropertyValue("TokenOutlineWidths").toString().split(",",QString::SkipEmptyParts);
	QStringList xs = propertyContainer_->getPropertyValue("TokenXs").toString().split(",",QString::SkipEmptyParts);
	QStringList ys = propertyContainer_->getPropertyValue("TokenYs").toString().split(",",QString::SkipEmptyParts);
	if	(	(sizes.size() != getNumTokens())
		||	(colors.size() != getNumTokens())
		||	(shapes.size() != getNumTokens())
		||	(outlines.size() != getNumTokens())
		||	(outlineWidths.size() != getNumTokens())
		||	(xs.size() != getNumTokens())
		||	(ys.size() != getNumTokens())
		)
	{
		//Stuff is in the process of being updated.
		return;
	}
	for(int i=0;i<numTokens;i++)
	{
		if(sizes[i] == "_")
			sizes[i] = "1.0";
		if(colors[i] == "_")
			colors[i] = getColor().name();
		if(shapes[i] == "_")
			shapes[i] = getTokenShape();
		if(outlines[i] == "_")
			outlines[i] = QString::number(int(getOutline()));
		if(outlineWidths[i] == "_")
			outlineWidths[i] = QString::number(getOutlineWidth());
		if(xs[i] == "_")
			xs[i] = QString("0");
		if(ys[i] == "_")
			ys[i] = QString("0");
	}
	QRect imageDimensions = getImageDims();
	QImage image(imageDimensions.width(),imageDimensions.height(),QImage::Format_ARGB32);
	image.fill(0);
	posOffset_ = QPoint(-imageDimensions.left(),-imageDimensions.top());

	for(int i=0;i<numTokens;i++)
	{	
		QColor color(colors[i]);
		double size = sizes[i].toDouble();
		QRect dimensions(tokenDimensions.x(),tokenDimensions.y(),tokenDimensions.width()*size,tokenDimensions.height()*size);
		QPoint tokenPosOffset = QPoint(dimensions.width()/2.0,dimensions.height()/2.0);
		QPoint offset = posOffset_+QPoint(xs[i].toInt(),ys[i].toInt())-tokenPosOffset;
		QPainter p(&image);
		QPen pen(color);
		p.setBrush(color);
		p.translate(offset);
		if(outlines[i].toInt())
		{
			p.setBrush(QColor(0,0,0,0));
			pen.setWidth(outlineWidths[i].toInt());
		}
		p.setPen(pen);
		p.setRenderHint(QPainter::Antialiasing, true);
		
		if(shapes[i] == "Ellipse")
		{
			p.drawEllipse(dimensions);
		}
		else if(shapes[i] == "Rectangle")
		{
			p.drawRect(dimensions);
		}
		else if(shapes[i] == "Diamond")
		{
			QPolygon diamond;
			int maxWPoint = dimensions.width()-1;
			int maxHPoint = dimensions.height()-1;
			diamond << QPoint(maxWPoint/2,0)
					<< QPoint(maxWPoint,maxHPoint/2)
					<< QPoint(maxWPoint/2,maxHPoint)
					<< QPoint(0,maxHPoint/2)
					<< QPoint(maxWPoint/2,0);
			p.drawPolygon(diamond);
		}

		p.end();
	}
	image_ = image;
	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* TokenFactoryGraphic::NewVisualElement()
{
	return new TokenFactoryGraphic;
}

QSharedPointer<Asset> TokenFactoryGraphic::Create()
{
	return QSharedPointer<Asset>(new TokenFactoryGraphic());
}

void TokenFactoryGraphic::reset()
{
	VisualElement::reset();
	foreach(QString listName,listNames_)
	{
		propertyContainer_->setPropertyValue(listName,"");
	}
	updateListSizes();
}

void TokenFactoryGraphic::postDeserialize()
{
	defaults_.push_back("1.0");
	defaults_.push_back(getColor().name());
	defaults_.push_back(getTokenShape());
	defaults_.push_back(QString::number(int(getOutline())));
	defaults_.push_back(QString::number(getOutlineWidth()));
	defaults_.push_back(QString::number(0));
	defaults_.push_back(QString::number(0));
	//Initialize values that are needed for drawing since VisualElement::postDeserialize() calls draw()
	updateListSizes();
	VisualElement::postDeserialize();

	//List properties are for information transfer only, hide them from the UI.
	foreach(QString listName,listNames_)
	{
		propertyContainer_->getProperty(listName)->setVisible(false);
	}
}

bool TokenFactoryGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	if(propertyContainer_->getPropertyValue("NumTokens").toInt() < 0)
	{
		addError("NumTokens must be greater than or equal to zero.");
		return false;
	}

	if(shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"") != "Ellipse")
	{
		addError("Non-Ellipse token trays are not yet supported.");
		return false;
	}
	return true;
}

void TokenFactoryGraphic::updateListSizes()
{
	int num = getNumTokens();
	foreach(QString listName,listNames_)
	{
		QStringList strList = propertyContainer_->getPropertyValue(listName).toString().split(",",QString::SkipEmptyParts);
		while(num < strList.size())
			strList.pop_back();
		while(num > strList.size())
			strList.push_back("_");
		propertyContainer_->setPropertyValue(listName,strList.join(","));
	}
}

//QPoint TokenFactoryGraphic::getTokenOffset(int index)
//{
//	QRect dims = getTrayDimensions();
//	int numTokens = getNumTokens();
//	double angle = double(index)*2.0*PI/double(numTokens);
//	angle += propertyContainer_->getPropertyValue("Phase").toInt()*PI/180.0;
//	//Keep angle in 0->2PI
//	int fullLoops = angle/(2*PI);
//	angle = angle-double(fullLoops*2*PI);
//	double h = dims.height();
//	double w = dims.width();
//	if((w <= 0) || (h <= 0))//Ideally, these cases would be allowed and just signify a straight line.  I'm not going to worry about this right now.
//		return QPoint(0,0);
//	double x=0;
//	double y=0;
//
//	QString trayShape = shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"");
//	if(trayShape == "Ellipse")
//	{
//		//Lots of algebra leads to the following x value where a line at angle (angle) intersects our ellipse
//		x = sqrt( (h*h*w*w) / (4.0* ((w*w*tan(angle)*tan(angle))+(h*h)) ) );
//		if((angle > PI/2.0) && (angle <= 3.0*PI/2.0))
//			x = -x;
//		//More algebra leads to the value of y below (where the line at angle (angle) intersects our ellipse).
//		double buff = (w*w*.25)-(x*x);
//		if(buff < 0) buff = 0;
//		y=-sqrt( h*h*(buff)/(w*w) );	//negative sine is because y is down in screen coordinates but up in standard coordinates
//		if(angle > PI)
//			y = -y;
//	}
//	else if(trayShape == "Rectangle")
//	{
//		
//	}
//	else if(trayShape == "Diamond")
//	{
//	
//	}
//	return QPoint(x,y);
//}


//void TokenFactoryGraphic::updateParameterLists()
//{
//	if(updatingParameterLists_)	//Changing properties can have the affect of getting this called recursively
//		return;				//This prevents that.
//	updatingParameterLists_ = true;
//
//	//If default values have changed, empty the list to which the default applies because the 
//	//init values in our list parameters are no longer accurate.  Ideally, we should just change the
//	//list in the init value, but I haven't done that yet.
//	if(getColor().name() != defaults_[1])
//	{
//		propertyContainer_->setPropertyValue("TokenColors","");
//	}
//	if(getTokenShape() != defaults_[2])
//	{
//		propertyContainer_->setPropertyValue("TokenShapes","");
//	}
//	if(QString::number(int(getOutline())) != defaults_[3])
//	{
//		propertyContainer_->setPropertyValue("TokenOutlines","");
//	}
//	if(QString::number(getOutlineWidth()) != defaults_[4])
//	{
//		propertyContainer_->setPropertyValue("TokenOutlineWidths","");
//	}
//
//	QVector<QStringList> propLists;
//	foreach(QString listName,listNames_)
//	{
//		propLists.push_back(propertyContainer_->getPropertyValue(listName).toString().split(",",QString::SkipEmptyParts));
//	}
//
//	//Assure that list lengths match the number of tokens.
//	int num = propertyContainer_->getPropertyValue("NumTokens").toInt();
//	for(int i=0;i<propLists.size();i++)
//	{
//		while(num < propLists[i].size())
//		{
//			propLists[i].pop_back();
//		}
//		while(num > propLists[i].size())
//		{
//			propLists[i].push_back(defaults_[i]);
//		}
//		
//	}
//	
//	//Write new changes over current lists.
//	QHash<int,QString>::iterator iter;
//	for(int i=0;i<deltaHashs_.size();i++)
//	{
//		for(iter = deltaHashs_[i]->begin();iter != deltaHashs_[i]->end();iter++)
//		{
//			if(propLists[i].size()<=iter.key())
//				continue;	//In case numTokens was decreased after something was changed on a high index token
//			int key = iter.key();
//			QString val = iter.value();
//			propLists[i][iter.key()] = iter.value();
//		}
//	}
//
//	//Rebuild property strings from lists.
//	for(int i=0;i<listNames_.size();i++)
//	{
//		propertyContainer_->setPropertyValue(listNames_[i],propLists[i].join(","));
//	}
//	updatingParameterLists_ = false;
//}

//int TokenFactoryGraphic::getMaxTokenDiam()
//{
//	int defaultOutlineWidth = getOutlineWidth();
//	int defaultWidth = getTokenWidth()+getOutlineWidth();
//	int defaultHeight = getTokenHeight()+getOutlineWidth();
//	int defaultTokenDiam = defaultWidth;
//	if(defaultHeight > defaultWidth) defaultTokenDiam = defaultHeight;
//	int maxDiam = defaultTokenDiam;
//	if(defaultHeight > defaultWidth) maxDiam = defaultHeight;
//	QStringList sizes = propertyContainer_->getPropertyValue("TokenSizes").toString().split(",",QString::SkipEmptyParts);
//	QStringList outlineWidths = propertyContainer_->getPropertyValue("TokenOutlineWidths").toString().split(",",QString::SkipEmptyParts);
//	if	(	(sizes.size() < getNumTokens())
//		||	(outlineWidths.size() < getNumTokens())
//		)
//		return defaultTokenDiam + defaultOutlineWidth  + 1;	//If this is the case, somethings being updated and calling draw and the system won't really need this data yet.
//	for(int i=0;i<getNumTokens();i++)
//	{
//		int oWidth = defaultOutlineWidth;
//		int diam = defaultTokenDiam;
//		if(sizes[i] != "_")
//			diam *= sizes[i].toDouble();
//		if(outlineWidths[i] != "_")
//			oWidth = outlineWidths[i].toInt();
//		int fullDiam = diam+oWidth;
//		if(fullDiam > maxDiam)
//			maxDiam = fullDiam;
//	}
//	return maxDiam+1;
//}

QRect TokenFactoryGraphic::getImageDims()
{
	int defaultOutlineWidth = getOutlineWidth();
	int defaultWidth = getTokenWidth();
	int defaultHeight = getTokenHeight();
	QStringList sizes = propertyContainer_->getPropertyValue("TokenSizes").toString().split(",",QString::SkipEmptyParts);
	QStringList outlineWidths = propertyContainer_->getPropertyValue("TokenOutlineWidths").toString().split(",",QString::SkipEmptyParts);
	QStringList xs = propertyContainer_->getPropertyValue("TokenXs").toString().split(",",QString::SkipEmptyParts);
	QStringList ys = propertyContainer_->getPropertyValue("TokenYs").toString().split(",",QString::SkipEmptyParts);
	
	if	(	(sizes.size() < getNumTokens())
		||	(outlineWidths.size() < getNumTokens())
		||	(xs.size() < getNumTokens())
		||	(ys.size() < getNumTokens())
		)
		return QRect();	//If this is the case, somethings being updated and calling draw and the system won't really need this data yet.

	QRect returnVal;
	for(int i=0;i<getNumTokens();i++)
	{
		QPoint currPoint;
		if(xs[i] != "_")
			currPoint.setX(xs[i].toInt());
		if(ys[i] != "_")
			currPoint.setY(ys[i].toInt());
		int oWidth = defaultOutlineWidth;
		int height = defaultHeight;
		int width = defaultWidth;
		if(sizes[i] != "_")
		{
			height *= sizes[i].toDouble();
			width *= sizes[i].toDouble();
		}
		if(outlineWidths[i] != "_")
			oWidth = outlineWidths[i].toInt();
		int halfWidth = (width+oWidth)/2;
		int halfHeight = (height+oWidth)/2;
		int minX = currPoint.x()-halfWidth;
		int minY = currPoint.y()-halfHeight;
		int maxX = currPoint.x()+halfWidth;
		int maxY = currPoint.y()+halfHeight;
		if(returnVal.isEmpty())
		{
			returnVal = QRect(QPoint(minX,minY),QPoint(maxX,maxY));
			continue;
		}
		if(minX < returnVal.left())
			returnVal.setLeft(minX);
		if(minY < returnVal.top())
			returnVal.setTop(minY);
		if(maxX > returnVal.right())
			returnVal.setRight(maxX);
		if(maxY > returnVal.bottom())
			returnVal.setBottom(maxY);
	}
	return returnVal;
}

}; //namespace Picto
