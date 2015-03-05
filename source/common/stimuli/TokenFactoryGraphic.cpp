#include <QPainter>
#include "TokenFactoryGraphic.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

#define PI 3.141592

	const QString TokenFactoryGraphic::type = "Token Factory Graphic";

/*! \brief Creates a new TokenFactoryGraphic object at the input position and with the input dimensions and color.
 *	\details Adds the following Properties:
 *	- TokenShape: Stores the default shape of the tokens (Ellipse, Rectangle or Diamond).
 *	- TokenSize: Stores the default size of the tokens.
 *	- NumTokens: Stores the number of tokens to be displayed.
 *	- Outline: Stores whether only the outline of the tokens will be shown by default.
 *	- OutlineWidth: Stores the thickness of the outline that will be shown if "Outline" is true.
 *	- TokenSizes: A comma separated list storing TokenSizes as a factor by which the default token size will be multiplied.  Unset values will be the default size.
 *	- TokenColors: A comma separated list storing TokenColors.  Unset values will be the default color.
 *	- TokenShapes: A comma separated list storing TokenShapes.  Unset values will be the default shape.
 *	- TokenOutlines: A comma separated list storing whether only the outlines of Tokens will be drawn.  Unset values will be according to the default Outline value.
 *	- TokenOutlineWidths: A comma separated list storing TokenOutlineWidgths.  Unset values will have the default OutlineWidth.
 *	- TokenXs: A comma separated list storing Token x positions with respect to the position of the TokenFactoryGraphic itself.
 *	- TokenYs: A comma separated list storing Token y positions with respect to the position of the TokenFactoryGraphic itself.
 */
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

/*! \brief Gets the default size of tokens.
 *	\note Only the width and height of the returned QRect are defined.
 */
QRect TokenFactoryGraphic::getTokenDimensions()
{
	return QRect(QPoint(),propertyContainer_->getPropertyValue("TokenSize").toSize());
}

/*! \brief Sets the default size of tokens.
 *	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
*/
void TokenFactoryGraphic::setTokenDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("TokenSize",dimensions.size());
}

/*! \brief Gets the default TokenShape as a string ("Ellipse", "Rectangle", or "Diamond").
*/
QString TokenFactoryGraphic::getTokenShape()
{
	return shapeList_.value(propertyContainer_->getPropertyValue("TokenShape").toInt(),"");
}

/*! \brief Sets the default TokenShape as a string ("Ellipse", "Rectangle", or "Diamond").
*/
void TokenFactoryGraphic::setTokenShape(QString shape)
{
	if(shapeList_.contains(shape))
		propertyContainer_->setPropertyValue("TokenShape",shapeList_.indexOf(shape));
}

/*! \brief Sets the number of tokens to be displayed.
*/
void TokenFactoryGraphic::setNumTokens(int num)
{
	if(num < 0)
		num = 0;
	propertyContainer_->setPropertyValue("NumTokens",num);
	updateListSizes();
}

/*! \brief Gets the number of tokens to be displayed.
*/
int TokenFactoryGraphic::getNumTokens()
{
	return propertyContainer_->getPropertyValue("NumTokens").toInt();
}

/*! \brief Gets whether only the outline of the tokens will be shown by default.
*/
bool TokenFactoryGraphic::getOutline()
{
	return propertyContainer_->getPropertyValue("Outline").toBool();
}

/*! \brief Sets whether only the outline of the tokens will be shown by default.
*/
void TokenFactoryGraphic::setOutline(bool on)
{
	propertyContainer_->setPropertyValue("Outline",on);
}

/*! \brief Gets the thickness of the outline that will be shown if "Outline" is true.
*/
int TokenFactoryGraphic::getOutlineWidth()
{
	return propertyContainer_->getPropertyValue("OutlineWidth").toInt();
}

/*! \brief Sets the thickness of the outline that will be shown if "Outline" is true.
*/
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
/*! \brief Sets the global x,y position of the token at the input index.
*/
void TokenFactoryGraphic::setTokenPos(int index,int x,int y)
{
	setTokenX(index,x);
	setTokenY(index,y);
}

/*! \brief Sets the local x,y position of the token at the input index with respect to the TokenFactoryGraphic position.
*/
void TokenFactoryGraphic::setTokenLocalPos(int index,int x,int y)
{
	setTokenLocalX(index,x);
	setTokenLocalY(index,y);
}

/*! \brief Sets the global x position of the token at the input index.
*/
void TokenFactoryGraphic::setTokenX(int index,int x)
{
	int offset = getPosition().x();
	setTokenLocalX(index,x-offset);
}

/*! \brief Sets the global y position of the token at the input index.
*/
void TokenFactoryGraphic::setTokenY(int index,int y)
{
	int offset = getPosition().y();
	setTokenLocalY(index,y-offset);
}
/*! \brief Sets the local x position of the token at the input index with respect to the TokenFactoryGraphic position.
*/
void TokenFactoryGraphic::setTokenLocalX(int index,int x)
{
	if(index < 0 || index >= getNumTokens())
		return;
	QStringList xs = propertyContainer_->getPropertyValue("TokenXs").toString().split(",",QString::SkipEmptyParts);
	xs[index] = QString::number(x);
	propertyContainer_->setPropertyValue("TokenXs",xs.join(","));
}

/*! \brief Sets the local y position of the token at the input index with respect to the TokenFactoryGraphic position.
*/
void TokenFactoryGraphic::setTokenLocalY(int index,int y)
{
	if(index < 0 || index >= getNumTokens())
		return;
	QStringList ys = propertyContainer_->getPropertyValue("TokenYs").toString().split(",",QString::SkipEmptyParts);
	ys[index] = QString::number(y);
	propertyContainer_->setPropertyValue("TokenYs",ys.join(","));
}

/*! \brief Sets the relative size of the token with the input index to size.  size is a factor by which the default token size will be multiplied for the token at the 
 *	input index.
 */
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

/*! \brief Sets the color of the token with the input index to the input red, green, blue, alpha values.
 */
void TokenFactoryGraphic::setTokenColor(int index, int r, int g, int b, int a)
{
	if(index < 0 || index >= getNumTokens())
		return;
	QStringList colors = propertyContainer_->getPropertyValue("TokenColors").toString().split(",",QString::SkipEmptyParts);
	colors[index] = QColor(r,g,b,a).name();
	propertyContainer_->setPropertyValue("TokenColors",colors.join(","));
}

/*! \brief Sets the color of the token with the input index to the input color QVariant.
 */
void TokenFactoryGraphic::setTokenColor(int index,QVariant color)
{
	QColor col = color.value<QColor>();
	setTokenColor(index,col.red(),col.green(),col.blue());
}

/*! \brief Sets the shape of the token with the input index to the input shape name ("Ellipse", "Rectangle", or "Diamond").
 */
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

/*! \brief Sets whether only the outline will be drawn for the token with the input index.
 */
void TokenFactoryGraphic::setTokenOutline(int index, bool on)
{
	if(index < 0 || index >= getNumTokens())
		return;
	QStringList outlines = propertyContainer_->getPropertyValue("TokenOutlines").toString().split(",",QString::SkipEmptyParts);
	outlines[index] = QString::number(int(on));
	propertyContainer_->setPropertyValue("TokenOutlines",outlines.join(","));
}

/*! \brief Sets the outline width to be drawn for the token with the input index (if only its outline is being drawn).
 */
void TokenFactoryGraphic::setTokenOutlineWidth(int index, int pixels)
{
	if(index < 0 || index >= getNumTokens())
		return;
	QStringList outlineWidths = propertyContainer_->getPropertyValue("TokenOutlineWidths").toString().split(",",QString::SkipEmptyParts);
	outlineWidths[index] = QString::number(pixels);
	propertyContainer_->setPropertyValue("TokenOutlineWidths",outlineWidths.join(","));
}

/*! \brief Returns the global x position component of the token at the input index.
 */
int TokenFactoryGraphic::getTokenX(int index)
{
	return getPosition().x() + getTokenLocalX(index);
}

/*! \brief Returns the global y position component of the token at the input index.
 */
int TokenFactoryGraphic::getTokenY(int index)
{
	return getPosition().y() + getTokenLocalY(index);
}

/*! \brief Returns the x position component of the token at the input index with respect to the TokenFactoryGraphic position.
 */
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

/*! \brief Returns the y position component of the token at the input index with respect to the TokenFactoryGraphic position.
 */
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

/*! \brief Returns the relative size factor of the token at the input index.
 */
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
/*! \brief Returns the red color component of the token at the input index.
 */
int TokenFactoryGraphic::getTokenRed(int index)
{
	return getTokenColor(index).red();
}
/*! \brief Returns the green color component of the token at the input index.
 */
int TokenFactoryGraphic::getTokenGreen(int index)
{
	return getTokenColor(index).green();
}
/*! \brief Returns the blue color component of the token at the input index.
 */
int TokenFactoryGraphic::getTokenBlue(int index)
{
	return getTokenColor(index).blue();
}
/*! \brief Returns the alpha color component of the token at the input index.
 */
int TokenFactoryGraphic::getTokenAlpha(int index)
{
	return getTokenColor(index).alpha();
}
/*! \brief Returns the color of the token at the input index.
 */
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
/*! \brief Returns whether only the outline is being drawn for the token at the input index.
 */
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
/*! \brief Returns the width of the outline being drawn for the token at the input index (when only its outline is drawn).
 */
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

/*! \brief Returns the shape of the token at the input index as a string ("Ellipse", "Rectangle", or "Diamond").
 */
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
	//Whenever a list's value is "_" it should juist used the default value
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

/*! \brief Creates a new TokenFactoryGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> TokenFactoryGraphic::Create()
{
	return QSharedPointer<Asset>(new TokenFactoryGraphic());
}

void TokenFactoryGraphic::enteredScope()
{
	foreach(QString listName,listNames_)
	{
		propertyContainer_->setPropertyValue(listName,"");
	}
	updateListSizes();
	VisualElement::enteredScope();
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

/*! \brief Extends VisualElement::validateObject() to make sure that the number of tokens is not negative.
 */
bool TokenFactoryGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	if(propertyContainer_->getPropertyValue("NumTokens").toInt() < 0)
	{
		addError("NumTokens must be greater than or equal to zero.");
		return false;
	}

	//This is a copy paste error, there is not Shape Property in this class.  Get rid of this line.
	if(shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"") != "Ellipse")
	{
		addError("Non-Ellipse token trays are not yet supported.");
		return false;
	}
	return true;
}

/*! \brief Updates the sizes of the lists stored in this object's Property values when necessary (ie. number of tokens changes).
*/
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

/*! \brief Calculates the minimum QImage size necessary to contain all of this TokenFactoryGraphic's tokens without clipping
 *	any of them and returns it in a QRect.
 */
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
