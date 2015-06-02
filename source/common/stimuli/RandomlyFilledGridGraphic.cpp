#include <QPainter>
#include <algorithm>
#include "RandomlyFilledGridGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

	const QString RandomlyFilledGridGraphic::type = "Randomly Filled Grid Graphic";

/*! \brief Constructs a RandomlyFilledGridGraphic at the input position with the input dimensions (only size component is used), with the primary color and
 *	secondary color2, the input number of horizontal and vertical squares in the grid, and the total number of color2 squares at a given frame.  Randomization
 *	happens once per updateFrameRate Frames.
 *	\details Adds the following Properties:
 *	- Color2: Every randomly filled grid graphic has two separate colors of squares, this is the second square color.
 *	- Size: The size of the grid graphic in pixels.
 *	- HorizontalSquares: The number of columns in the grid.
 *	- VerticalSquares: The number of rows in the grid.
 *	- Color2Squares: The number of squares that will have Color2 at a given time.
 *	- FramesPerUpdate: The number of frames that will pass before the grid is randomized and 
 *		colored squares are redistributed.
 *	\note If the number of squares requested doesn't evenly divide the total size, the graphic
 *	will not render properly.  It's the responsibility of the user to ensure this doesn't happen.
 *	This should be easlly fixed by putting a check into validateObject(), but we haven't done it yet.
 */
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

/*! \brief Creates a new RandomlyFilledGridGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> RandomlyFilledGridGraphic::Create()
{
	return QSharedPointer<Asset>(new RandomlyFilledGridGraphic());
}

QPoint RandomlyFilledGridGraphic::getPositionOffset()
{
	return posOffset_;
}

/*! \brief Builds a list of square colors that can be randomized and used to define the colors in the grid.
 *	\details This is currently called whenever draw() is called and does nothing unless a relevant Property
 *	value has changed since the last time this function was called.
*/
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

/*! \brief Implements VisualElement::draw() to randomize the colorList_ built in buildColorList() and draw it onto
 *	a grid for rendering.
 */
void RandomlyFilledGridGraphic::draw()
{
	int foregroundIndex;

	buildColorList();
	//int numColor2 = propertyContainer_->getPropertyValue("Color2Squares").toInt();
	int numHorizSquares = propertyContainer_->getPropertyValue("HorizontalSquares").toInt();
	int numVertSquares = propertyContainer_->getPropertyValue("VerticalSquares").toInt();

	QColor backgroundColor = propertyContainer_->getPropertyValue("Color").value<QColor>();
	QColor foregroundColor = propertyContainer_->getPropertyValue("Color2").value<QColor>();
	foregroundIndex = 1;

	std::random_shuffle(colorList_.begin(), colorList_.end());

	QRect dimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());

	//If there is nothign to paint, just return
	if (dimensions.width() == 0 || dimensions.height() == 0 || !dimensions.isValid())
	{
		return;
	}

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
	posOffset_ = QPoint(dimensions.width()/2.0,dimensions.height()/2.0);
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

/*! \brief Called whenever a Property changes to update the color list using buildColorList() or redraw the underlying image
 *	using draw() depending on which Property changed.
 */
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

/*! \brief Implements VisualElement::updateAnimation() to rerandomize and draw the grid graphic once every FramesPerUpdate.
*/
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
