#include <QPainter>
#include "CursorGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

	const QString CursorGraphic::type = "Cursor Graphic";

/*! \brief Creates a new CursorGraphic object whose position will be determined by the input SignalChannel and with the input color.
 *	\details The input SignalChannel must have x and y subcomponents.  This is checked in the constructor with assertions.
 */
CursorGraphic::CursorGraphic(QSharedPointer<SignalChannel> channel, QColor color)
: VisualElement(QPoint(0,0),color),
size_(16)
{
	// This is never serialized, so I didn't update it to use the new serialization routine.
	// It does bring to light the fact that most of our Asset objects must be deserialized
	// to have valid contents, which might be something worth fixing.
	positionChannel_ = channel;
	if(positionChannel_)
	{
		Q_ASSERT(positionChannel_->getSubchannels().contains("x"));
		Q_ASSERT(positionChannel_->getSubchannels().contains("y"));
	}
	//This object doesn't need a name, clear it.
	propertyContainer_->addProperty(QVariant::String, "Name", "");
	propertyContainer_->setContainerName(type);
	initializePropertiesToDefaults();
	setScalable(false);

	draw();
}

void CursorGraphic::draw()
{
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();
	
	QImage image(size_,size_,QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	QPen pen(color);
	int penWidth = float(size_)/5.0+.5;
	if(penWidth == 0)
		penWidth = 1;
	pen.setWidth(penWidth);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setPen(pen);
	p.setBrush(color);
	
	p.drawLine(size_/2,0,size_/2,size_-1);
	p.drawLine(0,size_/2,size_-1,size_/2);

	p.end();
	image_ = image;
	//image_ = image_.scaled(image_.size()/localZoom_);

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

/*! \brief Implements VisualElement::updateAnimation() to change the position of the cursor every frame based on the position SignalChannel's
 *	x, y values.
 */
void CursorGraphic::updateAnimation(int frame, QTime elapsedTime)
{
	Q_UNUSED(frame);
	Q_UNUSED(elapsedTime);

	if(shouldUpdateCompositingSurfaces_)
		updateCompositingSurfaces();
	
	if(positionChannel_)
	{
		int x = (int)positionChannel_->peekValue("x");
		int y = (int)positionChannel_->peekValue("y");	

		setPosition(QPoint(x,y));
	}
	draw();
}

QPoint CursorGraphic::getPositionOffset()
{
	return QPoint(size_/2,size_/2);
}

void CursorGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
}

bool CursorGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
