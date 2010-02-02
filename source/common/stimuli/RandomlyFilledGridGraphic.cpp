#include "RandomlyFilledGridGraphic.h"

#include <QPainter>
#include <algorithm>

namespace Picto {

RandomlyFilledGridGraphic::RandomlyFilledGridGraphic(QPoint position, QRect dimensions, 
													 QColor color1, QColor color2, 
													 int numHorizSquares, int numVertSquares,
													 int numColor1, bool animated, int updateFrameRate)
{
	propertyContainer_.setContainerName("Randomly Filled Grid Graphic");

	propertyContainer_.addProperty(Property(QVariant::Point,"Position",position));

	Property dimensionsProperty(QVariant::Rect,"Dimensions",dimensions);
	propertyContainer_.addProperty(dimensionsProperty);

	propertyContainer_.addProperty(Property(QVariant::Color,"Color1",color1));
	propertyContainer_.addProperty(Property(QVariant::Color,"Color2",color2));

	//NOTE: if the number of squares requested doesn't evenly divide the total size, the graphic
	//will not render properly.  It's the responsibility of the user to ensure this doesn't happen
	propertyContainer_.addProperty(Property(QVariant::Int,"Number of horizontal squares",numHorizSquares));
	propertyContainer_.addProperty(Property(QVariant::Int,"Number of vertical squares",numVertSquares));

	propertyContainer_.addProperty(Property(QVariant::Int,"Number of squares with color1",numColor1));

	propertyContainer_.addProperty(Property(QVariant::Bool,"Animated",animated));
	propertyContainer_.addProperty(Property(QVariant::Int,"Update frame rate",updateFrameRate));

	propertyContainer_.addProperty(Property(QVariant::String,"Name",""));

	colorList.clear();
	for(int i=0; i<numColor1; i++)
		colorList.push_back(0);

	for(int i=0; i<numHorizSquares*numVertSquares-numColor1; i++)
		colorList.push_back(1);

	draw();

	connect(&propertyContainer_,
		    SIGNAL(signalPropertyValueChanged(QString, QVariant)),
		    this,
			SLOT(slotPropertyValueChanged(QString, QVariant))
			);
}

void RandomlyFilledGridGraphic::draw()
{
	QColor backgroundColor;
	QColor foregroundColor;
	int foregroundIndex;

	int numColor1 = propertyContainer_.getPropertyValue("Number of squares with color1").toInt();
	int numHorizSquares = propertyContainer_.getPropertyValue("Number of horizontal squares").toInt();
	int numVertSquares = propertyContainer_.getPropertyValue("Number of vertical squares").toInt();


	if(numColor1 > numHorizSquares*numVertSquares/2)
	{
		backgroundColor = propertyContainer_.getPropertyValue("Color1").value<QColor>();
		foregroundColor = propertyContainer_.getPropertyValue("Color2").value<QColor>();
		foregroundIndex = 1;
	}
	else
	{
		backgroundColor = propertyContainer_.getPropertyValue("Color2").value<QColor>();
		foregroundColor = propertyContainer_.getPropertyValue("Color1").value<QColor>();
		foregroundIndex = 0;
	}


	std::random_shuffle(colorList.begin(), colorList.end());

	QRect dimensions = propertyContainer_.getPropertyValue("Dimensions").toRect();

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
	image.fill(backgroundColor.rgba());
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(foregroundColor);
	p.setPen(foregroundColor);

	int gridWidth = image.width()/numHorizSquares;
	int gridHeight = image.height()/numVertSquares;


	for(int x=0; x<numHorizSquares; x++)
	{
		for(int y=0; y<numVertSquares; y++)
		{
			if(colorList[y*numHorizSquares + x] != foregroundIndex)
				continue;
			p.drawRect(x*gridWidth,y*gridHeight,gridWidth,gridHeight);
		}
	}
	p.end();
	image_ = image;

	updateCompositingSurfaces();
}

void RandomlyFilledGridGraphic::slotPropertyValueChanged(QString propertyName,
											  QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}

void RandomlyFilledGridGraphic::updateAnimation(int frame, QTime elapsedTime)
{
	bool animated = propertyContainer_.getPropertyValue("Animated").toBool();
	if(!animated)
		return;

	int updateRate = propertyContainer_.getPropertyValue("Update frame rate").toInt();
	if(frame%updateRate == 0)
		draw();
	return;
}

}; //namespace Picto
