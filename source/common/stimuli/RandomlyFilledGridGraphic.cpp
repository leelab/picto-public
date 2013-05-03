#include <QPainter>
#include <algorithm>
#include "RandomlyFilledGridGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

const QString RandomlyFilledGridGraphic::type = "Randomly Filled Grid Graphic";

RandomlyFilledGridGraphic::RandomlyFilledGridGraphic(QPoint position, QRect dimensions, 
													 QColor color, QColor color2, 
													 int numHorizSquares, int numVertSquares,
													 int numColor2, int updateFrameRate)
: VisualElement(position,color),
	prevColor2Squares_(-1),
	prevHorizSquares_(-1),
	prevVertSquares_(-1)
{
	AddDefinableProperty(QVariant::Color,"Color2",color2);
	AddDefinableProperty(QVariant::Size,"Size",dimensions.size());
	//NOTE: if the number of squares requested doesn't evenly divide the total size, the graphic
	//will not render properly.  It's the responsibility of the user to ensure this doesn't happen.
	//This should be easlly fixed by putting a check into validateObject()
	AddDefinableProperty(QVariant::Int,"HorizontalSquares",numHorizSquares);
	AddDefinableProperty(QVariant::Int,"VerticalSquares",numVertSquares);

	AddDefinableProperty(QVariant::Int,"Color2Squares",numColor2);

	AddDefinableProperty(QVariant::Int,"FramesPerUpdate",updateFrameRate);
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
	int numColor2 = propertyContainer_->getPropertyValue("Color2Squares").toInt();
	int numHorizSquares = propertyContainer_->getPropertyValue("HorizontalSquares").toInt();
	int numVertSquares = propertyContainer_->getPropertyValue("VerticalSquares").toInt();

	//If none of the relevant parameters changed, we're done
	if((numColor2 == prevColor2Squares_) 
		&& (numHorizSquares == prevHorizSquares_)
		&&	(numVertSquares) == prevVertSquares_)
		return;
	prevColor2Squares_ = numColor2;
	prevHorizSquares_ = numHorizSquares;
	prevVertSquares_ = numVertSquares;

	colorList_.clear();
	colorList_.resize(numHorizSquares*numVertSquares);
	for(int i=0; i<numColor2; i++)
		colorList_[i] = 0;

	for(int i=numColor2; i<numHorizSquares*numVertSquares; i++)
		colorList_[i] = 1;
}

void RandomlyFilledGridGraphic::draw()
{
	int foregroundIndex;

	buildColorList();
	int numColor2 = propertyContainer_->getPropertyValue("Color2Squares").toInt();
	int numHorizSquares = propertyContainer_->getPropertyValue("HorizontalSquares").toInt();
	int numVertSquares = propertyContainer_->getPropertyValue("VerticalSquares").toInt();

	QColor backgroundColor = propertyContainer_->getPropertyValue("Color").value<QColor>();
	QColor foregroundColor = propertyContainer_->getPropertyValue("Color2").value<QColor>();
	foregroundIndex = 1;

	//! \todo use mersenne twister random number generator
	std::random_shuffle(colorList_.begin(), colorList_.end());

	QRect dimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
	image.fill(backgroundColor.rgba());
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(foregroundColor);
	p.setPen(foregroundColor);

	float gridWidth = 0;
	float gridHeight = 0;
	if(numHorizSquares && numVertSquares)
	{
		gridWidth = float(image.width())/float(numHorizSquares);
		gridHeight = float(image.height())/float(numVertSquares);
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
		propertyName == "Color2Squares")
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

	int updateRate = propertyContainer_->getPropertyValue("FramesPerUpdate").toInt();
	if(updateRate <= 0)
		return;
	if(frame%updateRate == 0)
		draw();
	return;
}

void RandomlyFilledGridGraphic::postDeserialize()
{
	buildColorList();
	VisualElement::postDeserialize();
}

bool RandomlyFilledGridGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
