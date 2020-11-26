#include <Qpainter>
#include "TargetFactory.h"
#include <sstream>
#include <iostream>

namespace Picto {

TargetFactory::TargetFactory(QPoint position, QRect dimensions, QColor color)
:
ControlTarget(),
currTargetIndex_(-1)
{
	shapeList_ << "Ellipse" << "Rectangle";
	AddDefinableProperty(PropertyContainer::enumTypeId(), "TargetShape", 0, "enumNames", shapeList_);
	AddDefinableProperty(QVariant::Size, "TargetSize", dimensions.size());

	AddDefinableProperty(QVariant::Int, "NumTargets", 0);

	//These are not be visible to the user in the designer window.
	listNames_.push_back("TargetSizes");
	listNames_.push_back("TargetColors");
	listNames_.push_back("TargetShapes");
	listNames_.push_back("TargetXs");
	listNames_.push_back("TargetYs");
	listNames_.push_back("TargetEnabled");
	foreach(QString listName, listNames_)
	{
		AddDefinableProperty(QVariant::String, listName, "");
	}
}

/*! \brief Gets the default size of targets.
*	\note Only the width and height of the returned QRect are defined.
*/
QRect TargetFactory::getTargetDimensions()
{
	return QRect(QPoint(), propertyContainer_->getPropertyValue("TargetSize").toSize());
}

/*! \brief Sets the default size of targets.
*	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
*/
void TargetFactory::setTargetDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("TargetSize", dimensions.size());
}

/*! \brief Gets the default TargetShape as a string ("Ellipse", "Rectangle", "Diamond" or "Triangle").
*/
QString TargetFactory::getTargetShape()
{
	return shapeList_.value(propertyContainer_->getPropertyValue("TargetShape").toInt(), "");
}

/*! \brief Sets the default TargetShape as a string ("Ellipse", "Rectangle", "Diamond" or "Triangle").
*/
void TargetFactory::setTargetShape(QString shape)
{
	if (shapeList_.contains(shape))
		propertyContainer_->setPropertyValue("TargetShape", shapeList_.indexOf(shape));
}

/*! \brief Sets the number of targets to be displayed.
*/
void TargetFactory::setNumTargets(int num)
{
	if (num < 0)
		num = 0;
	propertyContainer_->setPropertyValue("NumTargets", num);
	updateListSizes();
}

/*! \brief Gets the number of targets to be displayed.
*/
int TargetFactory::getNumTargets()
{
	return propertyContainer_->getPropertyValue("NumTargets").toInt();
}

QPoint TargetFactory::getPositionOffset()
{
	return posOffset_;
}
/*! \brief Sets the global x,y position of the target at the input index.
*/
void TargetFactory::setTargetPos(int index, int x, int y)
{
	setTargetX(index, x);
	setTargetY(index, y);
}

/*! \brief Sets the local x,y position of the target at the input index with respect to the TargetFactory position.
*/
void TargetFactory::setTargetLocalPos(int index, int x, int y)
{
	setTargetLocalX(index, x);
	setTargetLocalY(index, y);
}

/*! \brief Sets the global x position of the target at the input index.
*/
void TargetFactory::setTargetX(int index, int x)
{
	int offset = getPosition().x();
	setTargetLocalX(index, x - offset);
}

/*! \brief Sets the global y position of the target at the input index.
*/
void TargetFactory::setTargetY(int index, int y)
{
	int offset = getPosition().y();
	setTargetLocalY(index, y - offset);
}
/*! \brief Sets the local x position of the target at the input index with respect to the TargetFactory position.
*/
void TargetFactory::setTargetLocalX(int index, int x)
{
	if (index < 0 || index >= getNumTargets())
		return;
	QStringList xs = propertyContainer_->getPropertyValue("TargetXs").toString().split(",", QString::SkipEmptyParts);
	xs[index] = QString::number(x);
	propertyContainer_->setPropertyValue("TargetXs", xs.join(","));
}

/*! \brief Sets the local y position of the target at the input index with respect to the TargetFactory position.
*/
void TargetFactory::setTargetLocalY(int index, int y)
{
	if (index < 0 || index >= getNumTargets())
		return;
	QStringList ys = propertyContainer_->getPropertyValue("TargetYs").toString().split(",", QString::SkipEmptyParts);
	ys[index] = QString::number(y);
	propertyContainer_->setPropertyValue("TargetYs", ys.join(","));
}

/*! \brief Sets the relative size of the target with the input index to size. Size is a factor by which the default target size will be multiplied for the target at the
*	input index.
*/
void TargetFactory::setTargetSize(int index, double size)
{
	if (size > 1.0)
		size = 1.0;
	if (size < 0)
		size = 0.0;
	if (index < 0 || index >= getNumTargets())
		return;
	QStringList sizes = propertyContainer_->getPropertyValue("TargetSizes").toString().split(",", QString::SkipEmptyParts);
	sizes[index] = QString::number(size);
	propertyContainer_->setPropertyValue("TargetSizes", sizes.join(","));
}

/*! \brief Sets the color of the target with the input index to the input red, green, blue, alpha values.
*/
void TargetFactory::setTargetColor(int index, int r, int g, int b, int a)
{
	if (index < 0 || index >= getNumTargets())
		return;
	QStringList colors = propertyContainer_->getPropertyValue("TargetColors").toString().split(",", QString::SkipEmptyParts);
	QString alpha = QString::number(a);
	colors[index] = QColor(r, g, b, a).name() + "@" + alpha;
	propertyContainer_->setPropertyValue("TargetColors", colors.join(","));
}

/*! \brief Sets the color of the target with the input index to the input color QVariant.
*/
void TargetFactory::setTargetColor(int index, QVariant color)
{
	QColor col = color.value<QColor>();
	setTargetColor(index, col.red(), col.green(), col.blue(), col.alpha());
}

/*! \brief Sets the shape of the target with the input index to the input shape name ("Ellipse", "Rectangle", "Diamond" or "Triangle").
*/
void TargetFactory::setTargetShape(int index, QString shape)
{
	if (index < 0 || index >= getNumTargets())
		return;
	if (!shapeList_.contains(shape))
		return;
	QStringList shapes = propertyContainer_->getPropertyValue("TargetShapes").toString().split(",", QString::SkipEmptyParts);
	shapes[index] = shape;
	propertyContainer_->setPropertyValue("TargetShapes", shapes.join(","));
}

/*! \brief Sets whether this target is enabled or not.
*/
void TargetFactory::setTargetEnabled(int index, bool enabled)
{
	if (index < 0 || index >= getNumTargets())
		return;

	QStringList enable = propertyContainer_->getPropertyValue("TargetEnabled").toString().split(",", QString::SkipEmptyParts);
	enable[index] = QString::number(enabled);
	propertyContainer_->setPropertyValue("TargetEnabled", enable.join(","));
}

/*! \brief Returns the global x position component of the target at the input index.
*/
int TargetFactory::getTargetX(int index)
{
	return getPosition().x() + getTargetLocalX(index);
}

/*! \brief Returns the global y position component of the target at the input index.
*/
int TargetFactory::getTargetY(int index)
{
	return getPosition().y() + getTargetLocalY(index);
}

/*! \brief Returns the x position component of the target at the input index with respect to the TargetFactory position.
*/
int TargetFactory::getTargetLocalX(int index)
{
	QStringList xs = propertyContainer_->getPropertyValue("TargetXs").toString().split(",", QString::SkipEmptyParts);
	if (index < 0 || index >= xs.size())
		return 0;
	QString val = xs[index];
	if (val == "_")
		return 0;
	return val.toInt();
}

/*! \brief Returns the y position component of the target at the input index with respect to the TargetFactory position.
*/
int TargetFactory::getTargetLocalY(int index)
{
	QStringList ys = propertyContainer_->getPropertyValue("TargetYs").toString().split(",", QString::SkipEmptyParts);
	if (index < 0 || index >= ys.size())
		return 0;
	QString val = ys[index];
	if (val == "_")
		return 0;
	return val.toInt();
}

/*! \brief Returns the relative size factor of the target at the input index.
*/
double TargetFactory::getTargetSize(int index)
{
	if (index >= getNumTargets() || index < 0)
		return -1;
	QStringList sizes = propertyContainer_->getPropertyValue("TargetSizes").toString().split(",", QString::SkipEmptyParts);
	QString result = "_";
	if (sizes.size() >= index)
		result = sizes[index];
	if (result == "_")
		return 1.0;
	return result.toDouble();
}
/*! \brief Returns the red color component of the target at the input index.
*/
int TargetFactory::getTargetRed(int index)
{
	return getTargetColor(index).red();
}
/*! \brief Returns the green color component of the target at the input index.
*/
int TargetFactory::getTargetGreen(int index)
{
	return getTargetColor(index).green();
}
/*! \brief Returns the blue color component of the target at the input index.
*/
int TargetFactory::getTargetBlue(int index)
{
	return getTargetColor(index).blue();
}
/*! \brief Returns the alpha color component of the target at the input index.
*/
int TargetFactory::getTargetAlpha(int index)
{
	return getTargetColor(index).alpha();
}
/*! \brief Returns the color of the target at the input index.
*/
QColor TargetFactory::getTargetColor(int index)
{
	if (index > getNumTargets() || index < 0)
		return QColor();
	QStringList colors = propertyContainer_->getPropertyValue("TargetColors").toString().split(",", QString::SkipEmptyParts);

	QColor result = "_";
	int alpha = 255;
	int r, g, b;
	if (colors.size() >= index)
	{
		QString color = "_";
		color = colors[index];
		int n = color.indexOf("@");

		if (n > -1)
		{
			QString str = color.mid(n + 1);
			alpha = str.toInt();
			QString tmp = color.left(n);
			color = QString(tmp); //3 2-digits hexadecimal numbers
		}
		if (color == "_")
			return getColor();

		QString subStr1 = color.mid(1, 2);
		QByteArray ba1 = subStr1.toLatin1();
		const char *c_str1 = ba1.data();
		std::stringstream st1;
		st1 << std::hex << c_str1;
		st1 >> r;

		QString subStr2 = color.mid(3, 2);
		QByteArray ba2 = subStr2.toLatin1();
		const char *c_str2 = ba2.data();
		std::stringstream st2;
		st2 << std::hex << c_str2;
		st2 >> g;

		QString subStr3 = color.mid(5, 2);
		QByteArray ba3 = subStr3.toLatin1();
		const char *c_str3 = ba3.data();
		std::stringstream st3;
		st3 << std::hex << c_str3;
		st3 >> b;
	}

	result = QColor(r, g, b, alpha);
	return result;


}

/*! \brief Returns the shape of the target at the input index as a string ("Ellipse", "Rectangle", "Diamond" or "Triangle").
*/
QString TargetFactory::getTargetShape(int index)
{
	if (index > getNumTargets() || index < 0)
		return "";
	QStringList shapes = propertyContainer_->getPropertyValue("TargetShapes").toString().split(",", QString::SkipEmptyParts);
	QString result = "_";
	if (shapes.size() >= index)
		result = shapes[index];
	if (result == "_")
		return getTargetShape();
	return result;
}

/*! \brief Gets whether this target is enabled or not.
*/
bool TargetFactory::getTargetEnabled(int index)
{
	if (index > getNumTargets() || index < 0)
		return false;
	QStringList enabled = propertyContainer_->getPropertyValue("TargetEnabled").toString().split(",", QString::SkipEmptyParts);
	QString result = "_";
	if (enabled.size() >= index)
		result = enabled[index];
	if (result == "_")
		return true;
	return result.toInt();
}

void TargetFactory::draw()
{
	int numTargets = getNumTargets();
	QRect targetDimensions = QRect(QPoint(), propertyContainer_->getPropertyValue("TargetSize").toSize());
	//Make sure size of sizes,colors,shapes lists matches numTargets before drawing
	QStringList sizes = propertyContainer_->getPropertyValue("TargetSizes").toString().split(",", QString::SkipEmptyParts);
	QStringList colors = propertyContainer_->getPropertyValue("TargetColors").toString().split(",", QString::SkipEmptyParts);
	QStringList shapes = propertyContainer_->getPropertyValue("TargetShapes").toString().split(",", QString::SkipEmptyParts);
	QStringList xs = propertyContainer_->getPropertyValue("TargetXs").toString().split(",", QString::SkipEmptyParts);
	QStringList ys = propertyContainer_->getPropertyValue("TargetYs").toString().split(",", QString::SkipEmptyParts);
	QStringList enabled = propertyContainer_->getPropertyValue("TargetEnabled").toString().split(",", QString::SkipEmptyParts);
	if ((sizes.size() != getNumTargets())
		|| (colors.size() != getNumTargets())
		|| (shapes.size() != getNumTargets())
		|| (xs.size() != getNumTargets())
		|| (ys.size() != getNumTargets())
		|| (enabled.size() != getNumTargets())
		)
	{
		//Stuff is in the process of being updated.
		return;
	}
	//Whenever a list's value is "_" it should juist used the default value
	for (int i = 0; i<numTargets; i++)
	{
		if (sizes[i] == "_")
			sizes[i] = "1.0";
		if (colors[i] == "_")
			colors[i] = getColor().name();
		if (shapes[i] == "_")
			shapes[i] = getTargetShape();
		if (xs[i] == "_")
			xs[i] = QString("0");
		if (ys[i] == "_")
			ys[i] = QString("0");
	}

	QRect imageDimensions = getImageDims();
	QImage image(imageDimensions.width(), imageDimensions.height(), QImage::Format_ARGB32);
	image.fill(0);
	posOffset_ = QPoint(-imageDimensions.left(), -imageDimensions.top());

	for (int i = 0; i<numTargets; i++)
	{
		if (!getTargetEnabled(i))
			continue;

		double size = sizes[i].toDouble();
		QRect dimensions(targetDimensions.x(), targetDimensions.y(), targetDimensions.width()*size, targetDimensions.height()*size);
		QPoint TargetPosOffset = QPoint(dimensions.width() / 2.0, dimensions.height() / 2.0);
		QPoint offset = posOffset_ + QPoint(xs[i].toInt(), ys[i].toInt()) - TargetPosOffset;
		QPainter p(&image);
		QPen pen(colors[i]);
		pen.setColor(colors[i]);

		p.setBrush(QColor(0, 0, 0, 0));

		p.translate(offset);
		p.setPen(pen);
		p.setRenderHint(QPainter::Antialiasing, true);

		if (shapes[i] == "Ellipse")
			p.drawEllipse(dimensions);
		else if (shapes[i] == "Rectangle")
			p.drawRect(dimensions);
		
		p.end();
	}
	image_ = image;

	shouldUpdateCompositingSurfaces_ = true;
}

bool TargetFactory::contains(int x, int y)
{
	QStringList shapes = propertyContainer_->getPropertyValue("TargetShapes").toString().split(",", QString::SkipEmptyParts);
	QStringList sizes = propertyContainer_->getPropertyValue("TargetSizes").toString().split(",", QString::SkipEmptyParts);
	QStringList xs = propertyContainer_->getPropertyValue("TargetXs").toString().split(",", QString::SkipEmptyParts);
	QStringList ys = propertyContainer_->getPropertyValue("TargetYs").toString().split(",", QString::SkipEmptyParts);
	QRect targetDimensions = QRect(QPoint(), propertyContainer_->getPropertyValue("TargetSize").toSize());

	if (currTargetIndex_ == -1)
	{
		for (int i = 0; i < getNumTargets(); i++)
		{
			if (!getTargetEnabled(i))
				continue;
			if (sizes[i] == "_")
				sizes[i] = "1.0";
			if (shapes[i] == "_")
				shapes[i] = getTargetShape();
			if (xs[i] == "_")
				xs[i] = QString("0");
			if (ys[i] == "_")
				ys[i] = QString("0");

			double size = sizes[i].toDouble();

			if (shapes[i] == "Rectangle")
			{
				QRect dimensions = QRect(QPoint(), QSize(targetDimensions.width()*size, targetDimensions.height()*size));
				QPoint topLeft = QPoint(xs[i].toInt() - dimensions.width() / 2.0, ys[i].toInt() - dimensions.height() / 2.0);
				QRect myRect(topLeft.x(), topLeft.y(), dimensions.width(), dimensions.height());
				if (myRect.contains(x, y))
				{
					currTargetIndex_ = i;
					return true;
				}
			}
			else if (shapes[i] == "Ellipse")
			{
				int radius = targetDimensions.width()*size/2.0;
				QPoint center = QPoint(xs[i].toInt(), ys[i].toInt());
				QPoint offset = QPoint(x, y) - center;
				if ((((offset.rx()*offset.rx()) + (offset.ry()*offset.ry())) < (radius * radius)))
				{
					currTargetIndex_ = i;
					return true;
				}
			}
		}
	}
	else
	{
		if (!getTargetEnabled(currTargetIndex_))
		{
			currTargetIndex_ = -1;
			return false;
		}
		if (sizes[currTargetIndex_] == "_")
			sizes[currTargetIndex_] = "1.0";
		if (shapes[currTargetIndex_] == "_")
			shapes[currTargetIndex_] = getTargetShape();
		if (xs[currTargetIndex_] == "_")
			xs[currTargetIndex_] = QString("0");
		if (ys[currTargetIndex_] == "_")
			ys[currTargetIndex_] = QString("0");

		double size = sizes[currTargetIndex_].toDouble();

		if (shapes[currTargetIndex_] == "Rectangle")
		{
			QRect dimensions = QRect(QPoint(), QSize(targetDimensions.width()*size, targetDimensions.height()*size));
			QPoint topLeft = QPoint(xs[currTargetIndex_].toInt() - dimensions.width() / 2.0, ys[currTargetIndex_].toInt() - dimensions.height() / 2.0);
			QRect myRect(topLeft.x(), topLeft.y(), dimensions.width(), dimensions.height());

			if (myRect.contains(x, y))
				return true;
			else
			{
				currTargetIndex_ = -1;
				return false;
			}
		}
		else if (shapes[currTargetIndex_] == "Ellipse")
		{
			int radius = targetDimensions.width() * size / 2.0;
			QPoint center = QPoint(xs[currTargetIndex_].toInt(), ys[currTargetIndex_].toInt());
			QPoint offset = QPoint(x, y) - center;

			if ((((offset.rx()*offset.rx()) + (offset.ry()*offset.ry())) < (radius * radius)))
				return true;
			else
			{
				currTargetIndex_ = -1;
				return false;
			}
		}
	}
	return false;
}

/*! \brief Creates a new TargetFactory object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> TargetFactory::Create()
{
	return QSharedPointer<Asset>(new TargetFactory());
}

void TargetFactory::enteredScope()
{
	foreach(QString listName, listNames_)
	{
		propertyContainer_->setPropertyValue(listName, "");
	}
	updateListSizes();
	VisualElement::enteredScope();
	updateListSizes();
}

void TargetFactory::postDeserialize()
{
	defaults_.push_back("1.0");
	defaults_.push_back(getColor().name());
	defaults_.push_back(getTargetShape());
	defaults_.push_back(QString::number(0));
	defaults_.push_back(QString::number(0));
	defaults_.push_back(QString::number(1));
	//Initialize values that are needed for drawing since VisualElement::postDeserialize() calls draw()
	updateListSizes();
	VisualElement::postDeserialize();

	//List properties are for information transfer only, hide them from the UI.
	foreach(QString listName, listNames_)
	{
		propertyContainer_->getProperty(listName)->setVisible(false);
	}
	propertyContainer_->getProperty("Outside")->setVisible(false);
}

/*! \brief Extends VisualElement::validateObject() to make sure that the number of targets is not negative.
*/
bool TargetFactory::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if (!VisualElement::validateObject(xmlStreamReader))
		return false;
	if (propertyContainer_->getPropertyValue("NumTargets").toInt() < 0)
	{
		addError("NumTargets must be greater than or equal to zero.");
		return false;
	}
	return true;
}

/*! \brief Updates the sizes of the lists stored in this object's Property values when necessary (ie. number of targets changes).
*/
void TargetFactory::updateListSizes()
{
	int num = getNumTargets();
	foreach(QString listName, listNames_)
	{
		QStringList strList = propertyContainer_->getPropertyValue(listName).toString().split(",", QString::SkipEmptyParts);
		while (num < strList.size())
			strList.pop_back();
		while (num > strList.size())
			strList.push_back("_");
		propertyContainer_->setPropertyValue(listName, strList.join(","));
	}
}

/*! \brief Calculates the minimum QImage size necessary to contain all of this TargetFactory's targets without clipping
*	any of them and returns it in a QRect.
*/
QRect TargetFactory::getImageDims()
{
	int defaultWidth = getTargetWidth();
	int defaultHeight = getTargetHeight();
	QStringList sizes = propertyContainer_->getPropertyValue("TargetSizes").toString().split(",", QString::SkipEmptyParts);
	QStringList xs = propertyContainer_->getPropertyValue("TargetXs").toString().split(",", QString::SkipEmptyParts);
	QStringList ys = propertyContainer_->getPropertyValue("TargetYs").toString().split(",", QString::SkipEmptyParts);
	QStringList enabled = propertyContainer_->getPropertyValue("TargetEnabled").toString().split(",", QString::SkipEmptyParts);

	if ((sizes.size() < getNumTargets())
		|| (xs.size() < getNumTargets())
		|| (ys.size() < getNumTargets())
		|| (enabled.size() < getNumTargets())
		)
		return QRect();	//If this is the case, somethings being updated and calling draw and the system won't really need this data yet.

	QRect returnVal;
	for (int i = 0; i<getNumTargets(); i++)
	{
		if (!getTargetEnabled(i))
			continue;
		
		QPoint currPoint;
		if (xs[i] != "_")
			currPoint.setX(xs[i].toInt());
		if (ys[i] != "_")
			currPoint.setY(ys[i].toInt());
		int height = defaultHeight;
		int width = defaultWidth;
		if (sizes[i] != "_")
		{
			height *= sizes[i].toDouble();
			width *= sizes[i].toDouble();
		}
		int halfWidth = width / 2;
		int halfHeight = height / 2;
		int minX = currPoint.x() - halfWidth;
		int minY = currPoint.y() - halfHeight;
		int maxX = currPoint.x() + halfWidth;
		int maxY = currPoint.y() + halfHeight;
		if (returnVal.isEmpty())
		{
			returnVal = QRect(QPoint(minX, minY), QPoint(maxX, maxY));
			continue;
		}
		if (minX < returnVal.left())
			returnVal.setLeft(minX);
		if (minY < returnVal.top())
			returnVal.setTop(minY);
		if (maxX > returnVal.right())
			returnVal.setRight(maxX);
		if (maxY > returnVal.bottom())
			returnVal.setBottom(maxY);
	}
	return returnVal;
}

}; //namespace Picto