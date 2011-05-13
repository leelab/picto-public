#include "RandomlyFilledGridGraphic.h"

#include <QPainter>
#include <algorithm>

namespace Picto {

const QString RandomlyFilledGridGraphic::type = "Randomly Filled Grid Graphic";

RandomlyFilledGridGraphic::RandomlyFilledGridGraphic(QPoint position, QRect dimensions, 
													 QColor color1, QColor color2, 
													 int numHorizSquares, int numVertSquares,
													 int numColor1, bool animated, int updateFrameRate)
{
	propertyContainer_->setContainerName(type);

	propertyContainer_->setPropertyValue("Position",position);

	propertyContainer_->addProperty(QVariant::Rect,"Dimensions",dimensions);

	propertyContainer_->setPropertyValue("Color",color1);
	propertyContainer_->addProperty(QVariant::Color,"Color2",color2);

	//NOTE: if the number of squares requested doesn't evenly divide the total size, the graphic
	//will not render properly.  It's the responsibility of the user to ensure this doesn't happen
	propertyContainer_->addProperty(QVariant::Int,"Number of horizontal squares",numHorizSquares);
	propertyContainer_->addProperty(QVariant::Int,"Number of vertical squares",numVertSquares);

	propertyContainer_->addProperty(QVariant::Int,"Number of squares with color1",numColor1);

	propertyContainer_->addProperty(QVariant::Bool,"Animated",animated);
	propertyContainer_->addProperty(QVariant::Int,"Update frame rate",updateFrameRate);

	buildColorList();

	draw();

	connect(propertyContainer_.data(),
		    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
		    this,
			SLOT(slotPropertyValueChanged(QString, int, QVariant))
			);
}

VisualElement* RandomlyFilledGridGraphic::NewVisualElement()
{
	return new RandomlyFilledGridGraphic;
}


void RandomlyFilledGridGraphic::buildColorList()
{
	int numColor1 = propertyContainer_->getPropertyValue("Number of squares with color1").toInt();
	int numHorizSquares = propertyContainer_->getPropertyValue("Number of horizontal squares").toInt();
	int numVertSquares = propertyContainer_->getPropertyValue("Number of vertical squares").toInt();

	colorList_.clear();
	for(int i=0; i<numColor1; i++)
		colorList_.push_back(0);

	for(int i=0; i<numHorizSquares*numVertSquares-numColor1; i++)
		colorList_.push_back(1);
}

void RandomlyFilledGridGraphic::draw()
{
	QColor backgroundColor;
	QColor foregroundColor;
	int foregroundIndex;

	int numColor1 = propertyContainer_->getPropertyValue("Number of squares with color1").toInt();
	int numHorizSquares = propertyContainer_->getPropertyValue("Number of horizontal squares").toInt();
	int numVertSquares = propertyContainer_->getPropertyValue("Number of vertical squares").toInt();


	if(numColor1 > numHorizSquares*numVertSquares/2)
	{
		backgroundColor = propertyContainer_->getPropertyValue("Color1").value<QColor>();
		foregroundColor = propertyContainer_->getPropertyValue("Color2").value<QColor>();
		foregroundIndex = 1;
	}
	else
	{
		backgroundColor = propertyContainer_->getPropertyValue("Color2").value<QColor>();
		foregroundColor = propertyContainer_->getPropertyValue("Color1").value<QColor>();
		foregroundIndex = 0;
	}

	//! \todo use mersenne twister random number generator
	std::random_shuffle(colorList_.begin(), colorList_.end());

	QRect dimensions = propertyContainer_->getPropertyValue("Dimensions").toRect();

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
			if(colorList_[y*numHorizSquares + x] != foregroundIndex)
				continue;
			p.drawRect(x*gridWidth,y*gridHeight,gridWidth,gridHeight);
		}
	}
	p.end();
	image_ = image;

	updateCompositingSurfaces();
}

void RandomlyFilledGridGraphic::setHeight(int height)
{
	QRect origDims = getDimensions();
	origDims.setHeight(height);
	setDimensions(origDims);
}

void RandomlyFilledGridGraphic::setWidth(int width)
{
	QRect origDims = getDimensions();
	origDims.setWidth(width);
	setDimensions(origDims);
}

void RandomlyFilledGridGraphic::slotPropertyValueChanged(QString propertyName, int index,
											  QVariant) //propertyValue
{
	if(propertyName == "Number of horizontal squares" ||
		propertyName == "Number of vertical squares" ||
		propertyName == "Number of squares with color1")
	{
		buildColorList();
	}
	if(propertyName != "Position" && propertyName != "Name" 
		&& propertyName != "Update frame rate" && propertyName != "Animated")
	{
		draw();
	}
}

void RandomlyFilledGridGraphic::updateAnimation(int frame, QTime elapsedTime)
{
	Q_UNUSED(elapsedTime);

	bool animated = propertyContainer_->getPropertyValue("Animated").toBool();
	if(!animated)
		return;

	int updateRate = propertyContainer_->getPropertyValue("Update frame rate").toInt();
	if(frame%updateRate == 0)
		draw();
	return;
}

}; //namespace Picto
