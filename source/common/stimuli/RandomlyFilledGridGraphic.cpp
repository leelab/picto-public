#include <QPainter>
#include <algorithm>
#include "RandomlyFilledGridGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

const QString RandomlyFilledGridGraphic::type = "Randomly Filled Grid Graphic";

RandomlyFilledGridGraphic::RandomlyFilledGridGraphic(QPoint position, QRect dimensions, 
													 QColor color1, QColor color2, 
													 int numHorizSquares, int numVertSquares,
													 int numColor1, bool animated, int updateFrameRate)
: VisualElement(position,color1)
{
	AddDefinableProperty(QVariant::Color,"Color2",color2);
	AddDefinableProperty(QVariant::Rect,"Dimensions",dimensions);
	//NOTE: if the number of squares requested doesn't evenly divide the total size, the graphic
	//will not render properly.  It's the responsibility of the user to ensure this doesn't happen.
	//This should be easlly fixed by putting a check into validateObject()
	AddDefinableProperty(QVariant::Int,"HorizontalSquares",numHorizSquares);
	AddDefinableProperty(QVariant::Int,"VerticalSquares",numVertSquares);

	AddDefinableProperty(QVariant::Int,"Color1Squares",numColor1);

	AddDefinableProperty(QVariant::Bool,"Animated",animated);
	AddDefinableProperty(QVariant::Int,"FramesPerUpdate",updateFrameRate);

	


	//propertyContainer_->setContainerName(type);

	//propertyContainer_->setPropertyValue("Position",position);

	//propertyContainer_->addProperty(QVariant::Rect,"Dimensions",dimensions);

	//propertyContainer_->setPropertyValue("Color",color1);
	//propertyContainer_->addProperty(QVariant::Color,"Color2",color2);

	////NOTE: if the number of squares requested doesn't evenly divide the total size, the graphic
	////will not render properly.  It's the responsibility of the user to ensure this doesn't happen
	//propertyContainer_->addProperty(QVariant::Int,"HorizontalSquares",numHorizSquares);
	//propertyContainer_->addProperty(QVariant::Int,"VerticalSquares",numVertSquares);

	//propertyContainer_->addProperty(QVariant::Int,"Color1Squares",numColor1);

	//propertyContainer_->addProperty(QVariant::Bool,"Animated",animated);
	//propertyContainer_->addProperty(QVariant::Int,"FramesPerUpdate",updateFrameRate);

	//buildColorList();

	//draw();

	//connect(propertyContainer_.data(),
	//	    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
	//	    this,
	//		SLOT(slotPropertyValueChanged(QString, int, QVariant))
	//		);
}

VisualElement* RandomlyFilledGridGraphic::NewVisualElement()
{
	return new RandomlyFilledGridGraphic;
}

QSharedPointer<Asset> RandomlyFilledGridGraphic::Create()
{
	return QSharedPointer<Asset>(new RandomlyFilledGridGraphic());
}


void RandomlyFilledGridGraphic::buildColorList()
{
	int numColor1 = propertyContainer_->getPropertyValue("Color1Squares").toInt();
	int numHorizSquares = propertyContainer_->getPropertyValue("HorizontalSquares").toInt();
	int numVertSquares = propertyContainer_->getPropertyValue("VerticalSquares").toInt();

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

	int numColor1 = propertyContainer_->getPropertyValue("Color1Squares").toInt();
	int numHorizSquares = propertyContainer_->getPropertyValue("HorizontalSquares").toInt();
	int numVertSquares = propertyContainer_->getPropertyValue("VerticalSquares").toInt();


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

	int gridWidth = 0;
	int gridHeight = 0;
	if(numHorizSquares && numVertSquares)
	{
		gridWidth = image.width()/numHorizSquares;
		gridHeight = image.height()/numVertSquares;
	}


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

void RandomlyFilledGridGraphic::slotPropertyValueChanged(QString propertyName, int,
											  QVariant) //propertyValue
{
	if(propertyName == "HorizontalSquares" ||
		propertyName == "VerticalSquares" ||
		propertyName == "Color1Squares")
	{
		buildColorList();
	}
	if(propertyName != "Position" && propertyName != "Name" 
		&& propertyName != "FramesPerUpdate" && propertyName != "Animated")
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

	int updateRate = propertyContainer_->getPropertyValue("FramesPerUpdate").toInt();
	if(updateRate && frame%updateRate == 0)
		draw();
	return;
}

void RandomlyFilledGridGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
	buildColorList();
	draw();
}

bool RandomlyFilledGridGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
