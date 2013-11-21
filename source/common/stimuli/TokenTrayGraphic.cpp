#include <QPainter>
#include "TokenTrayGraphic.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

#define PI 3.141592
const QString TokenTrayGraphic::type = "Token Tray Graphic";

TokenTrayGraphic::TokenTrayGraphic(QPoint position, QRect dimensions, QColor color)
:
VisualElement(position,color)
{
	//updatingParameterLists_ = false;
	shapeList_ << "Ellipse";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"Shape",0,"enumNames",shapeList_);
	AddDefinableProperty(QVariant::Size,"Size",dimensions.size());

	shapeList_ << "Rectangle" << "Diamond";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"TokenShape",0,"enumNames",shapeList_);
	AddDefinableProperty(QVariant::Size,"TokenSize",dimensions.size());

	AddDefinableProperty(QVariant::Int,"NumTokens",1);
	AddDefinableProperty(QVariant::Int,"Phase",0);

	AddDefinableProperty(QVariant::Bool,"Outline",false);
	AddDefinableProperty(QVariant::Int,"OutlineWidth",0);

	//These should not be visible to the user in the designer window.  We'll have to make that happen at some point.
	listNames_.push_back("TokenSizes");
	listNames_.push_back("TokenColors");
	listNames_.push_back("TokenShapes");
	listNames_.push_back("TokenOutlines");
	listNames_.push_back("TokenOutlineWidths");
	foreach(QString listName,listNames_)
	{
		AddDefinableProperty(QVariant::String,listName,"");
	}

	//As of version 0.0.1, we have removed the TokenDimensions property
	//and replaced it with a TokenSize property.  See upgradeVersion() for details.
	AddDefinableObjectFactory("TokenDimensions",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));

	deltaHashs_.push_back(&sizeDelta_);
	deltaHashs_.push_back(&colorDelta_);
	deltaHashs_.push_back(&shapeDelta_);
	deltaHashs_.push_back(&outlineDelta_);
	deltaHashs_.push_back(&outlineWidthDelta_);
}
QRect TokenTrayGraphic::getTokenDimensions()
{
	return QRect(QPoint(),propertyContainer_->getPropertyValue("TokenSize").toSize());
}

void TokenTrayGraphic::setTokenDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("TokenSize",dimensions.size());
}

QString TokenTrayGraphic::getTokenShape()
{
	return shapeList_.value(propertyContainer_->getPropertyValue("TokenShape").toInt(),"");
}

void TokenTrayGraphic::setTokenShape(QString shape)
{
	if(shapeList_.contains(shape))
		propertyContainer_->setPropertyValue("TokenShape",shapeList_.indexOf(shape));
}

void TokenTrayGraphic::setNumTokens(int num)
{
	if(num < 0)
		num = 0;
	propertyContainer_->setPropertyValue("NumTokens",num);
	updateListSizes();
}
int TokenTrayGraphic::getNumTokens()
{
	return propertyContainer_->getPropertyValue("NumTokens").toInt();
}

QRect TokenTrayGraphic::getTrayDimensions()
{
	return QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
}

void TokenTrayGraphic::setTrayDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("Size",dimensions.size());
}

QString TokenTrayGraphic::getTrayShape()
{
	return shapeList_.value(propertyContainer_->getPropertyValue("TrayShape").toInt(),"");
}

void TokenTrayGraphic::setTrayShape(QString shape)
{
	if(shapeList_.contains(shape))
		propertyContainer_->setPropertyValue("TrayShape",shapeList_.indexOf(shape));
}

int TokenTrayGraphic::getPhase()
{
	return propertyContainer_->getPropertyValue("Phase").toInt();
}
void TokenTrayGraphic::setPhase(int phase)
{
	propertyContainer_->setPropertyValue("Phase",phase);
}

bool TokenTrayGraphic::getOutline()
{
	return propertyContainer_->getPropertyValue("Outline").toBool();
}

void TokenTrayGraphic::setOutline(bool on)
{
	propertyContainer_->setPropertyValue("Outline",on);
}

int TokenTrayGraphic::getOutlineWidth()
{
	return propertyContainer_->getPropertyValue("OutlineWidth").toInt();
}

void TokenTrayGraphic::setOutlineWidth(int pixels)
{
	if(pixels < 0)
		pixels = 0;
	propertyContainer_->setPropertyValue("OutlineWidth",pixels);
}

QPoint TokenTrayGraphic::getPositionOffset()
{
	return posOffset_;
}

void TokenTrayGraphic::upgradeVersion(QString deserializedVersion)
{
	VisualElement::upgradeVersion(deserializedVersion);
	if(deserializedVersion < "0.0.1")
	{
		//As of version 0.0.1, we have removed the TokenDimensions properties 
		//and replaced it with a TokenSize property.  We take advantage of the fact that the x,y
		//properties of the TokenDimensions rectangle were never used so we can just throw them out.

		//Get the old obsolete Dimensions Rect property
		QList<QSharedPointer<Asset>> oldRectProps = getGeneratedChildren("TokenDimensions");
		//Get the new Size property
		QList<QSharedPointer<Asset>> newSizeProps = getGeneratedChildren("TokenSize");
		if(oldRectProps.size() && newSizeProps.size())
		{
			Q_ASSERT((oldRectProps.size() == 1) && (newSizeProps.size() == 1));
			QSharedPointer<ObsoleteAsset> oldRectProp = oldRectProps.first().staticCast<ObsoleteAsset>();
			QSharedPointer<Property> newSizeProp = newSizeProps.first().staticCast<Property>();
			//Try getting the old rect from the two methods of serializing it that were used, attributes and string
			QStringList rectStringVals;
			if(oldRectProp->getAttributeValue("x").isEmpty())
			{
				rectStringVals = oldRectProp->getValue().split(",");
			}
			else
			{
				rectStringVals << oldRectProp->getAttributeValue("x") 
					<< oldRectProp->getAttributeValue("y") 
					<< oldRectProp->getAttributeValue("width") 
					<< oldRectProp->getAttributeValue("height");
			}
			Q_ASSERT(rectStringVals.size() == 4);
			QRect oldRect(	rectStringVals[0].toInt(),
							rectStringVals[1].toInt(),
							rectStringVals[2].toInt(),
							rectStringVals[3].toInt());
			newSizeProp->setValue(QSize(oldRect.width(),oldRect.height()));
		}
	}
}

void TokenTrayGraphic::setTokenSize(int index,double size)
{
	if(size > 1.0)
		size = 1.0;
	if(size < 0)
		size = 0.0;
	if(index >= getNumTokens() || index < 0)
		return;
	QStringList sizes = propertyContainer_->getPropertyValue("TokenSizes").toString().split(",",QString::SkipEmptyParts);
	sizes[index] = QString::number(size);
	propertyContainer_->setPropertyValue("TokenSizes",sizes.join(","));
}

void TokenTrayGraphic::setTokenColor(int index, int r, int g, int b, int a)
{
	if(index >= getNumTokens() || index < 0)
		return;
	QStringList colors = propertyContainer_->getPropertyValue("TokenColors").toString().split(",",QString::SkipEmptyParts);
	colors[index] = QColor(r,g,b,a).name();
	propertyContainer_->setPropertyValue("TokenColors",colors.join(","));
}

void TokenTrayGraphic::setTokenColor(int index,QVariant color)
{
	QColor col = color.value<QColor>();
	setTokenColor(index,col.red(),col.green(),col.blue());
}

void TokenTrayGraphic::setTokenShape(int index, QString shape)
{
	if(index >= getNumTokens() || index < 0)
		return;
	if(!shapeList_.contains(shape))
		return;
	QStringList shapes = propertyContainer_->getPropertyValue("TokenShapes").toString().split(",",QString::SkipEmptyParts);
	shapes[index] = shape;
	propertyContainer_->setPropertyValue("TokenShapes",shapes.join(","));
}
void TokenTrayGraphic::setTokenOutline(int index, bool on)
{
	if(index >= getNumTokens() || index < 0)
		return;
	QStringList outlines = propertyContainer_->getPropertyValue("TokenOutlines").toString().split(",",QString::SkipEmptyParts);
	outlines[index] = QString::number(int(on));
	propertyContainer_->setPropertyValue("TokenOutlines",outlines.join(","));
}
void TokenTrayGraphic::setTokenOutlineWidth(int index, int pixels)
{
	if(index >= getNumTokens() || index < 0)
		return;
	QStringList outlineWidths = propertyContainer_->getPropertyValue("TokenOutlineWidths").toString().split(",",QString::SkipEmptyParts);
	outlineWidths[index] = QString::number(pixels);
	propertyContainer_->setPropertyValue("TokenOutlineWidths",outlineWidths.join(","));
}

int TokenTrayGraphic::getTokenX(int index)
{
	return getTokenPosition(index).x();
}

int TokenTrayGraphic::getTokenY(int index)
{
	return getTokenPosition(index).y();
}

int TokenTrayGraphic::getTokenLocalX(int index)
{
	return getTokenOffset(index).x();
}

int TokenTrayGraphic::getTokenLocalY(int index)
{
	return getTokenOffset(index).y();
}

double TokenTrayGraphic::getTokenSize(int index)
{
	if(index >= getNumTokens() || index < 0)
		return -1;
	QStringList sizes = propertyContainer_->getPropertyValue("TokenSizes").toString().split(",",QString::SkipEmptyParts);
	QString result = "-1";
	if(sizes.size()>=index)
		result = sizes[index];
	if(result == "-1")
		return 1.0;
	return result.toDouble();
}
int TokenTrayGraphic::getTokenRed(int index)
{
	return getTokenColor(index).red();
}
int TokenTrayGraphic::getTokenGreen(int index)
{
	return getTokenColor(index).green();
}
int TokenTrayGraphic::getTokenBlue(int index)
{
	return getTokenColor(index).blue();
}
int TokenTrayGraphic::getTokenAlpha(int index)
{
	return getTokenColor(index).alpha();
}
QColor TokenTrayGraphic::getTokenColor(int index)
{
	if(index > getNumTokens() || index < 0)
		return QColor();
	QStringList colors = propertyContainer_->getPropertyValue("TokenColors").toString().split(",",QString::SkipEmptyParts);
	
	QString result = "-1";
	if(colors.size()>=index)
		result = colors[index];
	if(result == "-1")
		return getColor();;
	return QColor(result);
}
bool TokenTrayGraphic::getTokenOutline(int index)
{
	if(index >= getNumTokens() || index < 0)
		return false;
	QStringList outlines = propertyContainer_->getPropertyValue("TokenOutlines").toString().split(",",QString::SkipEmptyParts);
	QString result = "-1";
	if(outlines.size()>=index)
		result = outlines[index];
	if(result == "-1")
		return getOutline();
	return result.toInt();
}
int TokenTrayGraphic::getTokenOutlineWidth(int index)
{
	if(index >= getNumTokens() || index < 0)
		return -1;
	QStringList outlineWidths = propertyContainer_->getPropertyValue("TokenOutlineWidths").toString().split(",",QString::SkipEmptyParts);
	
	QString result = "-1";
	if(outlineWidths.size()>=index)
		result = outlineWidths[index];
	if(result == "-1")
		return getOutlineWidth();
	return result.toInt();
}
QString TokenTrayGraphic::getTokenShape(int index)
{
	if(index > getNumTokens() || index < 0)
		return "";
	QStringList shapes = propertyContainer_->getPropertyValue("TokenShapes").toString().split(",",QString::SkipEmptyParts);
	QString result = "-1";
	if(shapes.size()>=index)
		result = shapes[index];
	if(result == "-1")
		return getTokenShape();
	return result;
}


////This is always called every frame (even when draw is not) by the scene.
////We use it to update our properties based on changes from script calls
////since the last reset.  We don't do this in draw, because if the tray
////isn't visible, properties wouldn't be updated.
//void TokenTrayGraphic::updateAnimation(int frame, QTime elapsedTime)
//{
//	updateParameterLists();
//
//
//		int numTokens = getNumTokens();
//	QRect trayDimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
//	QRect tokenDimensions = propertyContainer_->getPropertyValue("TokenDimensions").toRect();
//	//Make sure size of sizes,colors,shapes lists matches numTokens before drawing
//	QStringList sizes = propertyContainer_->getPropertyValue("TokenSizes").toString().split(",",QString::SkipEmptyParts);
//	QStringList colors = propertyContainer_->getPropertyValue("TokenColors").toString().split(",",QString::SkipEmptyParts);
//	QStringList shapes = propertyContainer_->getPropertyValue("TokenShapes").toString().split(",",QString::SkipEmptyParts);
//	QStringList outlines = propertyContainer_->getPropertyValue("TokenOutlines").toString().split(",",QString::SkipEmptyParts);
//	QStringList outlineWidths = propertyContainer_->getPropertyValue("TokenOutlineWidths").toString().split(",",QString::SkipEmptyParts);
//	int maxTokenDiam= getMaxTokenDiam();
//	QRect imageDimensions(0,0,trayDimensions.width()+maxTokenDiam,trayDimensions.height()+maxTokenDiam);
//	QImage image(imageDimensions.width()+maxTokenDiam,imageDimensions.height()+maxTokenDiam,QImage::Format_ARGB32);
//	image.fill(0);
//	posOffset_ = QPoint(imageDimensions.width()/2.0,imageDimensions.height()/2.0);
//
//	for(int i=0;i<numTokens;i++)
//	{	
//		QColor color(colors[i]);
//		double size = sizes[i].toDouble();
//		QRect dimensions(tokenDimensions.x(),tokenDimensions.y(),tokenDimensions.width()*size,tokenDimensions.height()*size);
//		QPoint tokenPosOffset = QPoint(dimensions.width()/2.0,dimensions.height()/2.0);
//		QPoint offset = posOffset_+getTokenOffset(i)-tokenPosOffset;
//		QPainter p(&image);
//		QPen pen(color);
//		p.setBrush(color);
//		p.translate(offset);
//		if(outlines[i].toInt())
//		{
//			p.setBrush(QColor(0,0,0,0));
//			pen.setWidth(outlineWidths[i].toInt());
//		}
//		p.setPen(pen);
//		p.setRenderHint(QPainter::Antialiasing, true);
//		
//		if(shapes[i] == "Ellipse")
//		{
//			p.drawEllipse(dimensions);
//		}
//		else if(shapes[i] == "Rectangle")
//		{
//			p.drawRect(dimensions);
//		}
//		else if(shapes[i] == "Diamond")
//		{
//			QPolygon diamond;
//			int maxWPoint = dimensions.width()-1;
//			int maxHPoint = dimensions.height()-1;
//			diamond << QPoint(maxWPoint/2,0)
//					<< QPoint(maxWPoint,maxHPoint/2)
//					<< QPoint(maxWPoint/2,maxHPoint)
//					<< QPoint(0,maxHPoint/2)
//					<< QPoint(maxWPoint/2,0);
//			p.drawPolygon(diamond);
//		}
//
//		p.end();
//	}
//	image_ = image;
//	//updateCompositingSurfaces();
//
//	shouldUpdateCompositingSurfaces_ = true;
//
//	//Call parent version of the function.
//	VisualElement::updateAnimation(frame,elapsedTime);
//}

void TokenTrayGraphic::draw()
{
	//return;
	//if(updatingParameterLists_)
	//	return;	//This is being called recursively.  Leave.
	int numTokens = getNumTokens();
	QRect trayDimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
	QRect tokenDimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("TokenSize").toSize());
	//Make sure size of sizes,colors,shapes lists matches numTokens before drawing
	QStringList sizes = propertyContainer_->getPropertyValue("TokenSizes").toString().split(",",QString::SkipEmptyParts);
	QStringList colors = propertyContainer_->getPropertyValue("TokenColors").toString().split(",",QString::SkipEmptyParts);
	QStringList shapes = propertyContainer_->getPropertyValue("TokenShapes").toString().split(",",QString::SkipEmptyParts);
	QStringList outlines = propertyContainer_->getPropertyValue("TokenOutlines").toString().split(",",QString::SkipEmptyParts);
	QStringList outlineWidths = propertyContainer_->getPropertyValue("TokenOutlineWidths").toString().split(",",QString::SkipEmptyParts);
	if	(	(sizes.size() != getNumTokens())
		||	(colors.size() != getNumTokens())
		||	(shapes.size() != getNumTokens())
		||	(outlines.size() != getNumTokens())
		||	(outlineWidths.size() != getNumTokens())
		)
	{
		//Stuff is in the process of being updated.
		return;
	}
	for(int i=0;i<numTokens;i++)
	{
		if(sizes[i] == "-1")
			sizes[i] = "1.0";
		if(colors[i] == "-1")
			colors[i] = getColor().name();
		if(shapes[i] == "-1")
			shapes[i] = getTokenShape();
		if(outlines[i] == "-1")
			outlines[i] = QString::number(int(getOutline()));
		if(outlineWidths[i] == "-1")
			outlineWidths[i] = QString::number(getOutlineWidth());
	}
	int maxTokenDiam= getMaxTokenDiam();
	QRect imageDimensions(0,0,trayDimensions.width()+maxTokenDiam,trayDimensions.height()+maxTokenDiam);
	QImage image(imageDimensions.width()+maxTokenDiam,imageDimensions.height()+maxTokenDiam,QImage::Format_ARGB32);
	image.fill(0);
	posOffset_ = QPoint(imageDimensions.width()/2.0,imageDimensions.height()/2.0);

	for(int i=0;i<numTokens;i++)
	{	
		QColor color(colors[i]);
		double size = sizes[i].toDouble();
		QRect dimensions(tokenDimensions.x(),tokenDimensions.y(),tokenDimensions.width()*size,tokenDimensions.height()*size);
		QPoint tokenPosOffset = QPoint(dimensions.width()/2.0,dimensions.height()/2.0);
		QPoint offset = posOffset_+getTokenOffset(i)-tokenPosOffset;
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

VisualElement* TokenTrayGraphic::NewVisualElement()
{
	return new TokenTrayGraphic;
}

QSharedPointer<Asset> TokenTrayGraphic::Create()
{
	return QSharedPointer<Asset>(new TokenTrayGraphic());
}

void TokenTrayGraphic::enteredScope()
{
	foreach(QString listName,listNames_)
	{
		propertyContainer_->setPropertyValue(listName,"");
	}
	updateListSizes();
	VisualElement::enteredScope();
	updateListSizes();
}

void TokenTrayGraphic::postDeserialize()
{
	defaults_.push_back("1.0");
	defaults_.push_back(getColor().name());
	defaults_.push_back(getTokenShape());
	defaults_.push_back(QString::number(int(getOutline())));
	defaults_.push_back(QString::number(getOutlineWidth()));
	//Initialize values that are needed for drawing since VisualElement::postDeserialize() calls draw()
	updateListSizes();
	VisualElement::postDeserialize();

	//List properties are for information transfer only, hide them from the UI.
	foreach(QString listName,listNames_)
	{
		propertyContainer_->getProperty(listName)->setVisible(false);
	}
}

bool TokenTrayGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
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

void TokenTrayGraphic::updateListSizes()
{
	int num = getNumTokens();
	foreach(QString listName,listNames_)
	{
		QStringList strList = propertyContainer_->getPropertyValue(listName).toString().split(",",QString::SkipEmptyParts);
		while(num < strList.size())
			strList.pop_back();
		while(num > strList.size())
			strList.push_back("-1");
		propertyContainer_->setPropertyValue(listName,strList.join(","));
	}
}

QPoint TokenTrayGraphic::getTokenPosition(int index)
{
	return getPosition()+getTokenOffset(index);
}

QPoint TokenTrayGraphic::getTokenOffset(int index)
{
	QRect dims = getTrayDimensions();
	int numTokens = getNumTokens();
	double angle = double(index)*2.0*PI/double(numTokens);
	angle += propertyContainer_->getPropertyValue("Phase").toInt()*PI/180.0;
	//Keep angle in 0->2PI
	int fullLoops = angle/(2*PI);
	angle = angle-double(fullLoops*2*PI);
	double h = dims.height();
	double w = dims.width();
	if((w <= 0) || (h <= 0))//Ideally, these cases would be allowed and just signify a straight line.  I'm not going to worry about this right now.
		return QPoint(0,0);
	double x=0;
	double y=0;

	QString trayShape = shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"");
	if(trayShape == "Ellipse")
	{
		//Lots of algebra leads to the following x value where a line at angle (angle) intersects our ellipse
		x = sqrt( (h*h*w*w) / (4.0* ((w*w*tan(angle)*tan(angle))+(h*h)) ) );
		if((angle > PI/2.0) && (angle <= 3.0*PI/2.0))
			x = -x;
		//More algebra leads to the value of y below (where the line at angle (angle) intersects our ellipse).
		double buff = (w*w*.25)-(x*x);
		if(buff < 0) buff = 0;
		y=-sqrt( h*h*(buff)/(w*w) );	//negative sine is because y is down in screen coordinates but up in standard coordinates
		if(angle > PI)
			y = -y;
	}
	else if(trayShape == "Rectangle")
	{
		
	}
	else if(trayShape == "Diamond")
	{
	
	}
	return QPoint(x,y);
}


//void TokenTrayGraphic::updateParameterLists()
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

int TokenTrayGraphic::getMaxTokenDiam()
{
	int defaultOutlineWidth = getOutlineWidth();
	int defaultWidth = getTokenWidth()+getOutlineWidth();
	int defaultHeight = getTokenHeight()+getOutlineWidth();
	int defaultTokenDiam = defaultWidth;
	if(defaultHeight > defaultWidth) defaultTokenDiam = defaultHeight;
	int maxDiam = defaultTokenDiam;
	if(defaultHeight > defaultWidth) maxDiam = defaultHeight;
	QStringList sizes = propertyContainer_->getPropertyValue("TokenSizes").toString().split(",",QString::SkipEmptyParts);
	QStringList outlineWidths = propertyContainer_->getPropertyValue("TokenOutlineWidths").toString().split(",",QString::SkipEmptyParts);
	if	(	(sizes.size() < getNumTokens())
		||	(outlineWidths.size() < getNumTokens())
		)
		return defaultTokenDiam + defaultOutlineWidth  + 1;	//If this is the case, somethings being updated and calling draw and the system won't really need this data yet.
	for(int i=0;i<getNumTokens();i++)
	{
		int oWidth = defaultOutlineWidth;
		int diam = defaultTokenDiam;
		if(sizes[i] != "-1")
			diam *= sizes[i].toDouble();
		if(outlineWidths[i] != "-1")
			oWidth = outlineWidths[i].toInt();
		int fullDiam = diam+oWidth;
		if(fullDiam > maxDiam)
			maxDiam = fullDiam;
	}
	return maxDiam+1;
}

}; //namespace Picto
