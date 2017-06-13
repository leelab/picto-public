#include <QPainter>

#include "TextGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

	const QString TextGraphic::type = "Text Graphic";

	/*! \brief Creates a new TextGraphic object at the input position and with the input dimensions, color and text string.
	*	\details Adds the following Properties:
	*	- Size: Stores the size of the graphic (width, height).
	*	- Text: Stores the text that will be displayed in the graphic.
	*/
	TextGraphic::TextGraphic(QPoint position, QRect dimensions, QColor color, QString text)
		: VisualElement(position, color)
	{
		AddDefinableProperty(QVariant::Size, "Size", dimensions.size());
		AddDefinableProperty(QVariant::String, "Text", text);
		AddDefinableProperty(QVariant::Int, "FontSize", 12);
		if ((position != QPoint()) || (dimensions != QRect()) || (color != QColor()) || (text != QString()))
		{
			initializePropertiesToDefaults();
			//We set the properties as edited because we want this to serialize out and not be mistaken for a default value.
			propertyContainer_->getProperty("Position")->setEdited();
			propertyContainer_->getProperty("Color")->setEdited();
			propertyContainer_->getProperty("Size")->setEdited();
			propertyContainer_->getProperty("Text")->setEdited();
		}
	}

	void TextGraphic::draw()
	{

		QRect dimensions = QRect(QPoint(), propertyContainer_->getPropertyValue("Size").toSize());
		int fontSize = propertyContainer_->getPropertyValue("FontSize").toInt();

		QFont txtFont = QFont("Times", fontSize);
		QString str = propertyContainer_->getPropertyValue("Text").toString();

		QFontMetrics fm(txtFont);
		int strWidth = fm.width(str);
		int strHeight = fm.height();

		int rectWidth = 0;
		int rectHeight = 0;

		if (dimensions.width() != 0 && dimensions.height() != 0 &&  dimensions.isValid())
		{
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
			for (int i = 0; i < strList.size(); ++i)
			{
				if (fm.width(strList.at(i)) > strWidth)
				{
					strWidth = fm.width(strList.at(i));
					longestIndex = i;
				}
			}
			adjDimensions = fm.boundingRect(strList.at(longestIndex));
		}
		else
			adjDimensions = fm.boundingRect(str);

		if (rectWidth < strWidth || rectHeight < strHeight)
		{
			//auto-adjust size to text	
			if (adjDimensions.width() == 0 || adjDimensions.height() == 0 || !adjDimensions.isValid())
			{		
				if (dimensions.width() == 0 || dimensions.height() == 0 || !dimensions.isValid())
				{
					//use original font size only				
					dimensions.setWidth(fm.width(str));
					dimensions.setHeight(fm.height());
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

				dimensions.setWidth(strWidth);
				dimensions.setHeight(strHeight*nbLines);
			}
		}
		

		QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();
		
		QImage image(dimensions.width(), dimensions.height(), QImage::Format_ARGB32);
		image.fill(0);
		QPainter p(&image);
		p.setRenderHint(QPainter::Antialiasing, true);
		p.setBrush(color);
		p.setPen(color);
		//p.setFont(QFont("Times", fontSize));	
		p.setFont(txtFont);

		p.drawText(dimensions, Qt::AlignLeft | Qt::AlignVCenter, propertyContainer_->getPropertyValue("Text").toString());

		p.end();

		image_ = image;

		//updateCompositingSurfaces();

		shouldUpdateCompositingSurfaces_ = true;
	}

	/*! \brief Creates a new TextGraphic object and returns a shared Asset pointer to it.*/
	QSharedPointer<Asset> TextGraphic::Create()
	{
		return QSharedPointer<Asset>(new TextGraphic());
	}

	void TextGraphic::postDeserialize()
	{
		VisualElement::postDeserialize();
		setPropertyRuntimeEditable("Text");
	}

	bool TextGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
	{
		if (!VisualElement::validateObject(xmlStreamReader))
			return false;
		return true;
	}

}; //namespace Picto
