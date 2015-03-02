#include <QPainter>
#include "TokenTrayGraphic.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

#define PI 3.141592

const QString TokenTrayGraphic::type = "Token Tray Graphic";

/*! \brief Creates a new TokenTrayGraphic object at the input position and with the input dimensions and color.
 *	\details Adds the following Properties:
 *	- Shape: Stores the shape of the Token tray itself.  Tokens will be dispersed along a path of this shape.  Currently, only Ellipse is supported.
 *	- Size: Stores the size of the Token tray itself (width, height).
 *	- TokenShape: Stores the default shape of the individual tokens themselves (Ellipse, Rectangle or Diamond).
 *	- TokenSize: Stores the default size of the individual tokens themselves.
 *	- NumTokens: Stores the number of tokens to be displayed in this tray.
 *	- Phase: Stores the Phase in degrees at which the tokens will start being drawn starting from the right side and in a counter clockwise direction 
 *		(ie. 0 - 360 where 0 is along the x axis).
 *	- Outline: Stores whether only the outline of the individual tokens will be shown by default.
 *	- OutlineWidth: Stores the thickness of the outline that will be shown if "Outline" is true.
 *	- TokenSizes: A comma separated list storing individual TokenSizes as a factor by which the default token size will be multiplied.  Unset values will be the default size.
 *	- TokenColors: A comma separated list storing individual TokenColors.  Unset values will be the default color.
 *	- TokenShapes: A comma separated list storing individual TokenShapes.  Unset values will be the default shape.
 *	- TokenOutlines: A comma separated list storing whether only the outlines of individual Tokens will be drawn.  Unset values will be according to the default Outline value.
 *	- TokenOutlineWidths: A comma separated list storing individual TokenOutlineWidgths.  Unset values will have the default OutlineWidth.
 */
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
/*! \brief Gets the default size of individual tokens.
 *	\note Only the width and height of the returned QRect are defined.
 */
QRect TokenTrayGraphic::getTokenDimensions()
{
	return QRect(QPoint(),propertyContainer_->getPropertyValue("TokenSize").toSize());
}
/*! \brief Sets the default size of individual tokens.
 *	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
*/
void TokenTrayGraphic::setTokenDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("TokenSize",dimensions.size());
}
/*! \brief Gets the default TokenShape as a string ("Ellipse", "Rectangle", or "Diamond").
*/
QString TokenTrayGraphic::getTokenShape()
{
	return shapeList_.value(propertyContainer_->getPropertyValue("TokenShape").toInt(),"");
}

/*! \brief Sets the default TokenShape as a string ("Ellipse", "Rectangle", or "Diamond").
*/
void TokenTrayGraphic::setTokenShape(QString shape)
{
	if(shapeList_.contains(shape))
		propertyContainer_->setPropertyValue("TokenShape",shapeList_.indexOf(shape));
}

/*! \brief Sets the number of tokens to be displayed.
*/
void TokenTrayGraphic::setNumTokens(int num)
{
	if(num < 0)
		num = 0;
	propertyContainer_->setPropertyValue("NumTokens",num);
	updateListSizes();
}

/*! \brief Gets the number of tokens to be displayed.
*/
int TokenTrayGraphic::getNumTokens()
{
	return propertyContainer_->getPropertyValue("NumTokens").toInt();
}

/*! \brief Returns a QRect with the dimensions (width, height) of this entire token tray.
 *	\note The returned value contains no position information.  Only the Width, Height
 *	values should be used.
*/
QRect TokenTrayGraphic::getTrayDimensions()
{
	return QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
}

/*! \brief Sets the dimensions (width, height) of the entire tokey tray.
 *	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
*/
void TokenTrayGraphic::setTrayDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("Size",dimensions.size());
}

/*! \brief Gets the shape of the Token tray as a string (currently only "Ellipse" is supported).
*/
QString TokenTrayGraphic::getTrayShape()
{
	return shapeList_.value(propertyContainer_->getPropertyValue("TrayShape").toInt(),"");
}

/*! \brief Sets the default shape of the token tray as a string (currently only "Ellipse" is supported).
*/
void TokenTrayGraphic::setTrayShape(QString shape)
{
	if(shapeList_.contains(shape))
		propertyContainer_->setPropertyValue("TrayShape",shapeList_.indexOf(shape));
}

/*! \brief Gets the Phase in degrees at which the tokens start being drawn starting from the right side and in a counter clockwise direction 
 *		(ie. 0 - 360 where 0 is along the x axis).
 */
int TokenTrayGraphic::getPhase()
{
	return propertyContainer_->getPropertyValue("Phase").toInt();
}
/*! \brief Sets the Phase in degrees at which the tokens start being drawn starting from the right side and in a counter clockwise direction 
 *		(ie. 0 - 360 where 0 is along the x axis).
 */
void TokenTrayGraphic::setPhase(int phase)
{
	propertyContainer_->setPropertyValue("Phase",phase);
}

/*! \brief Gets whether only the outline of the individual tokens will be shown by default.
*/
bool TokenTrayGraphic::getOutline()
{
	return propertyContainer_->getPropertyValue("Outline").toBool();
}

/*! \brief Sets whether only the outline of the individual tokens will be shown by default.
*/
void TokenTrayGraphic::setOutline(bool on)
{
	propertyContainer_->setPropertyValue("Outline",on);
}

/*! \brief Gets the thickness of the outline that will be shown if "Outline" is true.
*/
int TokenTrayGraphic::getOutlineWidth()
{
	return propertyContainer_->getPropertyValue("OutlineWidth").toInt();
}

/*! \brief Sets the thickness of the outline that will be shown if "Outline" is true.
*/
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

/*! \brief Sets the relative size of the token with the input index to size.  size is a factor by which the default token size will be multiplied for the token at the 
 *	input index.
 */
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
/*! \brief Sets the color of the token with the input index to the input red, green, blue, alpha values.
 */
void TokenTrayGraphic::setTokenColor(int index, int r, int g, int b, int a)
{
	if(index >= getNumTokens() || index < 0)
		return;
	QStringList colors = propertyContainer_->getPropertyValue("TokenColors").toString().split(",",QString::SkipEmptyParts);
	colors[index] = QColor(r,g,b,a).name();
	propertyContainer_->setPropertyValue("TokenColors",colors.join(","));
}

/*! \brief Sets the color of the token with the input index to the input color QVariant.
 */
void TokenTrayGraphic::setTokenColor(int index,QVariant color)
{
	QColor col = color.value<QColor>();
	setTokenColor(index,col.red(),col.green(),col.blue());
}

/*! \brief Sets the shape of the token with the input index to the input shape name ("Ellipse", "Rectangle", or "Diamond").
 */
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
/*! \brief Sets whether only the outline will be drawn for the token with the input index.
 */
void TokenTrayGraphic::setTokenOutline(int index, bool on)
{
	if(index >= getNumTokens() || index < 0)
		return;
	QStringList outlines = propertyContainer_->getPropertyValue("TokenOutlines").toString().split(",",QString::SkipEmptyParts);
	outlines[index] = QString::number(int(on));
	propertyContainer_->setPropertyValue("TokenOutlines",outlines.join(","));
}

/*! \brief Sets the outline width to be drawn for the token with the input index (if only its outline is being drawn).
 */
void TokenTrayGraphic::setTokenOutlineWidth(int index, int pixels)
{
	if(index >= getNumTokens() || index < 0)
		return;
	QStringList outlineWidths = propertyContainer_->getPropertyValue("TokenOutlineWidths").toString().split(",",QString::SkipEmptyParts);
	outlineWidths[index] = QString::number(pixels);
	propertyContainer_->setPropertyValue("TokenOutlineWidths",outlineWidths.join(","));
}

/*! \brief Returns the x position component of the token at the input index.
 */
int TokenTrayGraphic::getTokenX(int index)
{
	return getTokenPosition(index).x();
}

/*! \brief Returns the y position component of the token at the input index.
 */
int TokenTrayGraphic::getTokenY(int index)
{
	return getTokenPosition(index).y();
}

/*! \brief Returns the x position component of the token at the input index with respect to the center of the token tray.
 */
int TokenTrayGraphic::getTokenLocalX(int index)
{
	return getTokenOffset(index).x();
}

/*! \brief Returns the y position component of the token at the input index with respect to the center of the token tray.
 */
int TokenTrayGraphic::getTokenLocalY(int index)
{
	return getTokenOffset(index).y();
}

/*! \brief Returns the relative size factor of the token at the input index.
 */
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
/*! \brief Returns the red color component of the token at the input index.
 */
int TokenTrayGraphic::getTokenRed(int index)
{
	return getTokenColor(index).red();
}
/*! \brief Returns the green color component of the token at the input index.
 */
int TokenTrayGraphic::getTokenGreen(int index)
{
	return getTokenColor(index).green();
}
/*! \brief Returns the blue color component of the token at the input index.
 */
int TokenTrayGraphic::getTokenBlue(int index)
{
	return getTokenColor(index).blue();
}
/*! \brief Returns the alpha color component of the token at the input index.
 */
int TokenTrayGraphic::getTokenAlpha(int index)
{
	return getTokenColor(index).alpha();
}
/*! \brief Returns the color of the token at the input index.
 */
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
/*! \brief Returns whether only the outline is being drawn for the token at the input index.
 */
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
/*! \brief Returns the width of the outline being drawn for the token at the input index (when only its outline is drawn).
 */
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

/*! \brief Returns the shape of the token at the input index as a string ("Ellipse", "Rectangle", or "Diamond").
 */
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
	//Whenever a list's value is -1 it should juist used the default value
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

/*! \brief Creates a new TokenTrayGraphic object and returns a shared Asset pointer to it.*/
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

/*! \brief Extends VisualElement::validateObject() to make sure that the number of tokens is not negative and that only
 *	elliptical token trays are being used since other shapes are not yet supported.
 */
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

/*! \brief Updates the sizes of the lists stored in this object's Property values when necessary (ie. number of tokens changes).
*/
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

/*! \brief Returns the x,y position of the token at the input index.*/
QPoint TokenTrayGraphic::getTokenPosition(int index)
{
	return getPosition()+getTokenOffset(index);
}

/*! \brief Returns the x,y position of the token at the input index relative to the center of the token tray.*/
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

/*! \brief Returns the maximum possible diameter of a token.  This is used to calculate the size of the QImage that we need in order to be sure
 *	that none of the token graphics gets clipped.
 */
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
