#include <QPainter>
#include "LabelFactoryGraphic.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

#define PI 3.141592

	const QString LabelFactoryGraphic::type = "Label Factory Graphic";

	/*! \brief Creates a new LabelFactoryGraphic object at the input position and with the input text content and color.
	*	\details Adds the following Properties:
	*	- LabelContent: Stores the default Content of the Labels.
	*	- LabelSize: Stores the default size of the Labels.
	*	- NumLabels: Stores the number of Labels to be displayed.
	*	- LabelColors: A comma separated list storing LabelColors.  Unset values will be the default color.
	*	- LabelContents: A comma separated list storing LabelContents.  Unset values will be the default Content.
	*	- LabelXs: A comma separated list storing Label x positions with respect to the position of the LabelFactoryGraphic itself.
	*	- LabelYs: A comma separated list storing Label y positions with respect to the position of the LabelFactoryGraphic itself.
	*/
	LabelFactoryGraphic::LabelFactoryGraphic(QPoint position, QRect dimensions, QColor color)
		:
		VisualElement(position, color)
	{
		//updatingParameterLists_ = false;		
		AddDefinableProperty(QVariant::String, "LabelFontSize", "0");
		AddDefinableProperty(QVariant::String, "LabelContent", Content_);
		AddDefinableProperty(QVariant::Size, "LabelSize", dimensions.size());
		AddDefinableProperty(QVariant::Int, "NumLabels", 0);

		//These are not be visible to the user in the designer window.
		listNames_.push_back("LabelSizes");
		listNames_.push_back("LabelColors");
		listNames_.push_back("LabelContents");
		listNames_.push_back("LabelFontSizes");
		listNames_.push_back("LabelXs");
		listNames_.push_back("LabelYs");
		foreach(QString listName, listNames_)
		{
			AddDefinableProperty(QVariant::String, listName, "");
		}

		//deltaHashs_.push_back(&sizeDelta_);
		//deltaHashs_.push_back(&colorDelta_);
		//deltaHashs_.push_back(&ContentDelta_);
		//deltaHashs_.push_back(&xDelta_);
		//deltaHashs_.push_back(&yDelta_);
	}

	/*! \brief Gets the default size of Labels.
	*	\note Only the width and height of the returned QRect are defined.
	*/
	QRect LabelFactoryGraphic::getLabelDimensions()
	{
		return QRect(QPoint(), propertyContainer_->getPropertyValue("LabelSize").toSize());
	}

	/*! \brief Sets the default size of Labels.
	*	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
	*/
	void LabelFactoryGraphic::setLabelDimensions(QRect dimensions)
	{
		propertyContainer_->setPropertyValue("LabelSize", dimensions.size());
	}

	/*! \brief Gets the default LabelContent as a string list.
	*/
	QString LabelFactoryGraphic::getLabelContent()
	{
		return propertyContainer_->getPropertyValue("LabelContent").toString();

	}

	/*! \brief Sets the default LabelContent as a string list.
	*/
	void LabelFactoryGraphic::setLabelContent(QString content)
	{
		if (content != "")
			propertyContainer_->setPropertyValue("LabelContent", content);
	}
	/*! \brief Sets the number of Labels to be displayed.
	*/
	void LabelFactoryGraphic::setNumLabels(int num)
	{
		if (num < 0)
			num = 0;
		propertyContainer_->setPropertyValue("NumLabels", num);
		updateListSizes();
	}

	/*! \brief Gets the number of Labels to be displayed.
	*/
	int LabelFactoryGraphic::getNumLabels()
	{
		return propertyContainer_->getPropertyValue("NumLabels").toInt();
	}
	/*! \brief Gets the default TokenShape as a string ("Ellipse", "Rectangle", or "Diamond").
	*/
	QString LabelFactoryGraphic::getLabelFontSize()
	{
		return propertyContainer_->getPropertyValue("LabelFontSize").toString();
	}

	/*! \brief Sets the default TokenShape as a string ("Ellipse", "Rectangle", or "Diamond").
	*/
	void LabelFactoryGraphic::setLabelFontSize(QString fontsize)
	{
		if (fontsize != "")
			propertyContainer_->setPropertyValue("LabelFontSize", fontsize);
	}

	QPoint LabelFactoryGraphic::getPositionOffset()
	{
		return posOffset_;
	}
	/*! \brief Sets the global x,y position of the Label at the input index.
	*/
	void LabelFactoryGraphic::setLabelPos(int index, int x, int y)
	{
		setLabelX(index, x);
		setLabelY(index, y);
	}

	/*! \brief Sets the local x,y position of the Label at the input index with respect to the LabelFactoryGraphic position.
	*/
	void LabelFactoryGraphic::setLabelLocalPos(int index, int x, int y)
	{
		setLabelLocalX(index, x);
		setLabelLocalY(index, y);
	}

	/*! \brief Sets the global x position of the Label at the input index.
	*/
	void LabelFactoryGraphic::setLabelX(int index, int x)
	{
		int offset = getPosition().x();
		setLabelLocalX(index, x - offset);
	}

	/*! \brief Sets the global y position of the Label at the input index.
	*/
	void LabelFactoryGraphic::setLabelY(int index, int y)
	{
		int offset = getPosition().y();
		setLabelLocalY(index, y - offset);
	}
	/*! \brief Sets the local x position of the Label at the input index with respect to the LabelFactoryGraphic position.
	*/
	void LabelFactoryGraphic::setLabelLocalX(int index, int x)
	{
		if (index < 0 || index >= getNumLabels())
			return;
		QStringList xs = propertyContainer_->getPropertyValue("LabelXs").toString().split(",", QString::SkipEmptyParts);
		xs[index] = QString::number(x);
		propertyContainer_->setPropertyValue("LabelXs", xs.join(","));
	}

	/*! \brief Sets the local y position of the Label at the input index with respect to the LabelFactoryGraphic position.
	*/
	void LabelFactoryGraphic::setLabelLocalY(int index, int y)
	{
		if (index < 0 || index >= getNumLabels())
			return;
		QStringList ys = propertyContainer_->getPropertyValue("LabelYs").toString().split(",", QString::SkipEmptyParts);
		ys[index] = QString::number(y);
		propertyContainer_->setPropertyValue("LabelYs", ys.join(","));
	}

	/*! \brief Sets the relative size of the Label with the input index to size.  size is a factor by which the default Label size will be multiplied for the Label at the
	*	input index (multiplication is done in draw())
	*/
	void LabelFactoryGraphic::setLabelSize(int index, double size)
	{
		if (size > 1.0)
			size = 1.0;
		if (size < 0)
			size = 0.0;
		if (index < 0 || index >= getNumLabels())
			return;
		QStringList sizes = propertyContainer_->getPropertyValue("LabelSizes").toString().split(",", QString::SkipEmptyParts);
		sizes[index] = QString::number(size);
		propertyContainer_->setPropertyValue("LabelSizes", sizes.join(","));
	}

	/*! \brief Sets the color of the Label with the input index to the input red, green, blue, alpha values.
	*/
	void LabelFactoryGraphic::setLabelColor(int index, int r, int g, int b, int a)
	{
		if (index < 0 || index >= getNumLabels())
			return;
		QStringList colors = propertyContainer_->getPropertyValue("LabelColors").toString().split(",", QString::SkipEmptyParts);
		colors[index] = QColor(r, g, b, a).name();
		propertyContainer_->setPropertyValue("LabelColors", colors.join(","));
	}

	/*! \brief Sets the color of the Label with the input index to the input color QVariant.
	*/
	void LabelFactoryGraphic::setLabelColor(int index, QVariant color)
	{
		QColor col = color.value<QColor>();
		setLabelColor(index, col.red(), col.green(), col.blue());
	}

	/*! \brief Sets the shape of the token with the input index to the input shape name ("Ellipse", "Rectangle", or "Diamond").
	*/
	void LabelFactoryGraphic::setLabelFontSize(int index, QString fontsize)
	{
		if (index < 0 || index >= getNumLabels())
			return;

		QStringList fontsizes = propertyContainer_->getPropertyValue("LabelFontSizes").toString().split(",", QString::SkipEmptyParts);
		fontsizes[index] = fontsize;
		propertyContainer_->setPropertyValue("LabelFontSizes", fontsizes.join(","));
	}

	/*! \brief Sets the Content of the Label with the input index to the input Content name ("Ellipse", "Rectangle", or "Diamond").
	*/
	void LabelFactoryGraphic::setLabelContent(int index, QString Content)
	{
		if (index < 0 || index >= getNumLabels())
			return;

		QStringList Contents = propertyContainer_->getPropertyValue("LabelContents").toString().split(",", QString::SkipEmptyParts);
		Contents[index] = Content;
		propertyContainer_->setPropertyValue("LabelContents", Contents.join(","));
	}

	/*! \brief Returns the Content of the Label at the input index as a string ("Ellipse", "Rectangle", or "Diamond").
	*/
	QString LabelFactoryGraphic::getLabelContent(int index)
	{
		if (index > getNumLabels() || index < 0)
			return "";
		QStringList Contents = propertyContainer_->getPropertyValue("LabelContents").toString().split(",", QString::SkipEmptyParts);
		QString result = "_";
		if (Contents.size() >= index)
			result = Contents[index];
		if (result == "_")
			return getLabelContent();
		return result;
	}

	/*! \brief Returns the shape of the token at the input index as a string ("Ellipse", "Rectangle", or "Diamond").
	*/
	QString LabelFactoryGraphic::getLabelFontSize(int index)
	{
		if (index > getNumLabels() || index < 0)
			return "0";
		QStringList fontsizes = propertyContainer_->getPropertyValue("LabelFontSizes").toString().split(",", QString::SkipEmptyParts);
		QString result = "_";
		if (fontsizes.size() >= index)
			result = fontsizes[index];
		if (result == "_")
			return getLabelFontSize();
		return result;
	}

	/*! \brief Returns the global x position component of the Label at the input index.
	*/
	int LabelFactoryGraphic::getLabelX(int index)
	{
		return getPosition().x() + getLabelLocalX(index);
	}

	/*! \brief Returns the global y position component of the Label at the input index.
	*/
	int LabelFactoryGraphic::getLabelY(int index)
	{
		return getPosition().y() + getLabelLocalY(index);
	}

	/*! \brief Returns the x position component of the Label at the input index with respect to the LabelFactoryGraphic position.
	*/
	int LabelFactoryGraphic::getLabelLocalX(int index)
	{
		QStringList xs = propertyContainer_->getPropertyValue("LabelXs").toString().split(",", QString::SkipEmptyParts);
		if (index < 0 || index >= xs.size())
			return 0;
		QString val = xs[index];
		if (val == "_")
			return 0;
		return val.toInt();
	}

	/*! \brief Returns the y position component of the Label at the input index with respect to the LabelFactoryGraphic position.
	*/
	int LabelFactoryGraphic::getLabelLocalY(int index)
	{
		QStringList ys = propertyContainer_->getPropertyValue("LabelYs").toString().split(",", QString::SkipEmptyParts);
		if (index < 0 || index >= ys.size())
			return 0;
		QString val = ys[index];
		if (val == "_")
			return 0;
		return val.toInt();
	}

	/*! \brief Returns the relative size factor of the Label at the input index.
	*/
	double LabelFactoryGraphic::getLabelSize(int index)
	{
		if (index >= getNumLabels() || index < 0)
			return -1;
		QStringList sizes = propertyContainer_->getPropertyValue("LabelSizes").toString().split(",", QString::SkipEmptyParts);
		QString result = "_";
		if (sizes.size() >= index)
			result = sizes[index];
		if (result == "_")
			return 1.0;
		return result.toDouble();
	}
	/*! \brief Returns the red color component of the Label at the input index.
	*/
	int LabelFactoryGraphic::getLabelRed(int index)
	{
		return getLabelColor(index).red();
	}
	/*! \brief Returns the green color component of the Label at the input index.
	*/
	int LabelFactoryGraphic::getLabelGreen(int index)
	{
		return getLabelColor(index).green();
	}
	/*! \brief Returns the blue color component of the Label at the input index.
	*/
	int LabelFactoryGraphic::getLabelBlue(int index)
	{
		return getLabelColor(index).blue();
	}
	/*! \brief Returns the alpha color component of the Label at the input index.
	*/
	int LabelFactoryGraphic::getLabelAlpha(int index)
	{
		return getLabelColor(index).alpha();
	}
	/*! \brief Returns the color of the Label at the input index.
	*/
	QColor LabelFactoryGraphic::getLabelColor(int index)
	{
		if (index > getNumLabels() || index < 0)
			return QColor();
		QStringList colors = propertyContainer_->getPropertyValue("LabelColors").toString().split(",", QString::SkipEmptyParts);

		QString result = "_";
		if (colors.size() >= index)
			result = colors[index];
		if (result == "_")
			return getColor();;
		return QColor(result);
	}



	void LabelFactoryGraphic::draw()
	{

		int numLabels = getNumLabels();
		QRect trayDimensions = QRect(QPoint(), propertyContainer_->getPropertyValue("Size").toSize());
		QRect LabelDimensions = QRect(QPoint(), propertyContainer_->getPropertyValue("LabelSize").toSize());

		//Make sure size of sizes,colors,Contents lists matches numLabels before drawing
		QStringList sizes = propertyContainer_->getPropertyValue("LabelSizes").toString().split(",", QString::SkipEmptyParts);
		QStringList colors = propertyContainer_->getPropertyValue("LabelColors").toString().split(",", QString::SkipEmptyParts);
		QStringList Contents = propertyContainer_->getPropertyValue("LabelContents").toString().split(",", QString::SkipEmptyParts);
		QStringList xs = propertyContainer_->getPropertyValue("LabelXs").toString().split(",", QString::SkipEmptyParts);
		QStringList ys = propertyContainer_->getPropertyValue("LabelYs").toString().split(",", QString::SkipEmptyParts);
		QStringList fontSizes = propertyContainer_->getPropertyValue("LabelFontSizes").toString().split(",", QString::SkipEmptyParts);

		if ((sizes.size() != getNumLabels() && fontSizes.size() != getNumLabels()) //either size or font size
			|| (colors.size() != getNumLabels())
			|| (Contents.size() != getNumLabels())
			|| (xs.size() != getNumLabels())
			|| (ys.size() != getNumLabels())
			)
		{
			//Stuff is in the process of being updated.
			return;
		}

		//Whenever a list's value is "_" it should just used the default value
		for (int i = 0; i < numLabels; i++)
		{
			if (sizes.size() > i && sizes[i] == "_")
				sizes[i] = "1.0";
			if (colors[i] == "_")
				colors[i] = getColor().name();
			if (Contents[i] == "_")
				Contents[i] = getLabelContent();
			if (xs[i] == "_")
				xs[i] = QString("0");
			if (ys[i] == "_")
				ys[i] = QString("0");
			if (fontSizes.size() > i && fontSizes[i] == "_")
				fontSizes[i] = QString("0");
		}
		QRect imageDimensions = getImageDims();
		QImage image(imageDimensions.width() * 2, imageDimensions.height() * 3, QImage::Format_ARGB32);
		image.fill(0);
		posOffset_ = QPoint(-imageDimensions.left(), -imageDimensions.top());

		for (int i = 0; i < numLabels; i++)
		{
			if (Contents[i] != "")
			{
				QColor color(colors[i]);
				double size = 1.; //default
				if (sizes.size() > i  && sizes[i] != "_")
					size = sizes[i].toDouble();

				QRect Tmpdimensions(LabelDimensions.x(), LabelDimensions.y(), LabelDimensions.width()*size, LabelDimensions.height()*size);
				QPoint LabelPosOffset = QPoint(Tmpdimensions.width() / 2.0, Tmpdimensions.height() / 2.0);
				QPoint offset = posOffset_ + QPoint(xs[i].toInt(), ys[i].toInt()) - LabelPosOffset;

				// for each text, re-adjust to font size if size was not set in the script or if it is null or too small
				int fontsize = getLabelFontSize().toInt();
				if (fontSizes.size() > i  && fontSizes[i] != "_" && fontSizes[i] != "0")
					fontsize = fontSizes[i].toInt();

				QFont txtFont = QFont("Times", fontsize);
				QString str = Contents[i];

				QRect dimensions = adjustSizeToFont(str, txtFont, Tmpdimensions);

				QPainter p(&image);
				QPen pen(color);
				p.setBrush(color);
				p.translate(offset);
				p.setPen(pen);
				p.setRenderHint(QPainter::Antialiasing, true);

				//if (Contents[i] != "")
				{
					p.setFont(txtFont);
					p.drawText(dimensions, Qt::AlignLeft | Qt::AlignVCenter, Contents[i]);
				}


				p.end();
			}
		}
		image_ = image;
		//updateCompositingSurfaces();

		shouldUpdateCompositingSurfaces_ = true;
	}
	
	QRect LabelFactoryGraphic::adjustSizeToFont(QString str, QFont txtFont, QRect dimensions)
	{
		QRect returnVal(dimensions.left(), dimensions.top(), dimensions.width(), dimensions.height());

		QFontMetrics fm(txtFont);
		int strWidth = fm.boundingRect(str).width();
		int strHeight = fm.boundingRect(str).height();

		int rectWidth = 0;
		int rectHeight = 0;

		if (dimensions.width() != 0 && dimensions.height() != 0 && dimensions.isValid())
		{
			//default is defined size (percent of the default size)
			rectWidth = dimensions.width();
			rectHeight = dimensions.height();
		}

		QRect adjDimensions; //dimension of the longest subString in the text

		//find all the return carriage \n in the text and keep the width of the longest substring
		QStringList strList = str.split('\n');
		if (strList.size() > 0)
		{
			int longestIndex = 0;
			strWidth = 0;
			for (int j = 0; j < strList.size(); ++j)
			{
				if (fm.width(strList.at(j)) > strWidth)
				{
					strWidth = fm.width(strList.at(j));
					longestIndex = j;
				}
			}
			adjDimensions = fm.boundingRect(strList.at(longestIndex));
		}
		else
			adjDimensions = fm.boundingRect(str);

		if (rectWidth < strWidth || rectHeight < strHeight)
		{
			//auto-adjust size to text if size was not defined or if it is null or too small for the text
			if (adjDimensions.width() == 0 || adjDimensions.height() == 0 || !adjDimensions.isValid())
			{
				if (dimensions.width() == 0 || dimensions.height() == 0 || !dimensions.isValid())
				{
					//use original font size only				
					returnVal.setWidth(fm.boundingRect(str).width());
					returnVal.setHeight(fm.boundingRect(str).height());
				}

				if (strWidth > 0)
				{
					//auto-adjust font-size to specified size with a limit of 60% to 125%
					double factor = dimensions.width() / strWidth;
					if ((factor < 1) || factor > 1.25)
					{
						if (factor < 0.6)
							factor = 0.6;
						else if (factor > 1.5)
							factor = 1.5;

						txtFont.setPointSizeF(txtFont.pointSizeF()*factor);
					}
				}
			}
			else
			{
				//number of lines in the text
				int nbLines = 1;
				nbLines = nbLines + str.count('\n');

				returnVal.setWidth(strWidth);
				returnVal.setHeight(strHeight*nbLines);
			}
		}
		return returnVal;
	}

	/*! \brief Creates a new LabelFactoryGraphic object and returns a shared Asset pointer to it.*/
	QSharedPointer<Asset> LabelFactoryGraphic::Create()
	{
		return QSharedPointer<Asset>(new LabelFactoryGraphic());
	}

	void LabelFactoryGraphic::enteredScope()
	{
		foreach(QString listName, listNames_)
		{
			propertyContainer_->setPropertyValue(listName, "");
		}
		updateListSizes();
		VisualElement::enteredScope();
		updateListSizes();
	}

	void LabelFactoryGraphic::postDeserialize()
	{
		defaults_.push_back("1.0");
		defaults_.push_back(getColor().name());
		defaults_.push_back(getLabelContent());
		defaults_.push_back(QString::number(0));
		defaults_.push_back(QString::number(0));
		//Initialize values that are needed for drawing since VisualElement::postDeserialize() calls draw()
		updateListSizes();
		VisualElement::postDeserialize();

		//List properties are for information transfer only, hide them from the UI.
		foreach(QString listName, listNames_)
		{
			propertyContainer_->getProperty(listName)->setVisible(false);
		}
	}

	/*! \brief Extends VisualElement::validateObject() to make sure that the number of Labels is not negative.
	*/
	bool LabelFactoryGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
	{
		if (!VisualElement::validateObject(xmlStreamReader))
			return false;
		if (propertyContainer_->getPropertyValue("NumLabels").toInt() < 0)
		{
			addError("NumLabels must be greater than or equal to zero.");
			return false;
		}
			
		return true;
	}

	/*! \brief Updates the sizes of the lists stored in this object's Property values when necessary (ie. number of Labels changes).
	*/
	void LabelFactoryGraphic::updateListSizes()
	{
		int num = getNumLabels();
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

	/*! \brief Calculates the minimum QImage size necessary to contain all of this LabelFactoryGraphic's Labels without clipping
	*	any of them and returns it in a QRect.
	*/
	QRect LabelFactoryGraphic::getImageDims()
	{
		int defaultWidth = getLabelWidth();
		int defaultHeight = getLabelHeight();
		QStringList sizes = propertyContainer_->getPropertyValue("LabelSizes").toString().split(",", QString::SkipEmptyParts);
		QStringList xs = propertyContainer_->getPropertyValue("LabelXs").toString().split(",", QString::SkipEmptyParts);
		QStringList ys = propertyContainer_->getPropertyValue("LabelYs").toString().split(",", QString::SkipEmptyParts);
		QStringList fontSizes = propertyContainer_->getPropertyValue("LabelFontSizes").toString().split(",", QString::SkipEmptyParts);
		QStringList Contents = propertyContainer_->getPropertyValue("LabelContents").toString().split(",", QString::SkipEmptyParts);

		if ((sizes.size() < getNumLabels() && fontSizes.size() < getNumLabels())
			|| (xs.size() < getNumLabels())
			|| (ys.size() < getNumLabels())
			|| (Contents.size() < getNumLabels())
			)
			return QRect();	//If this is the case, somethings being updated and calling draw and the system won't really need this data yet.

		QRect returnVal;
		for (int i = 0; i<getNumLabels(); i++)
		{
			QPoint currPoint;
			if (xs[i] != "_")
				currPoint.setX(xs[i].toInt());
			if (ys[i] != "_")
				currPoint.setY(ys[i].toInt());
			int height = defaultHeight;
			int width = defaultWidth;
			if (sizes.size() > i && sizes[i] != "_")
			{
				height *= sizes[i].toDouble();
				width *= sizes[i].toDouble();
			}

			int fontsize = getLabelFontSize().toInt();
			QString str = Contents[i];
			if (fontSizes.size() > i && fontSizes[i] != "_" && fontSizes[i] != "0")
				fontsize = fontSizes[i].toInt();

			QFont txtFont = QFont("Times", fontsize);			
			
			QRect Tmpdimensions(0., 0., width, height);
			QRect dimensions = adjustSizeToFont(str, txtFont, Tmpdimensions);

			width = dimensions.width();
			height = dimensions.height();

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
