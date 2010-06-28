#include "CursorGraphic.h"

#include <QPainter>

namespace Picto {

const QString CursorGraphic::type = "Cursor Graphic";

CursorGraphic::CursorGraphic(QSharedPointer<SignalChannel> channel, QColor color)
{
	positionChannel_ = channel;
	if(positionChannel_)
	{
		Q_ASSERT(positionChannel_->getSubchannels().contains("xpos"));
		Q_ASSERT(positionChannel_->getSubchannels().contains("ypos"));
	}

	propertyContainer_.setContainerName(type);
	propertyContainer_.setPropertyValue("Position",QPoint(0,0));
	propertyContainer_.setPropertyValue("Color",color);

	draw();

	connect(&propertyContainer_,
		    SIGNAL(signalPropertyValueChanged(QString, QVariant)),
		    this,
			SLOT(slotPropertyValueChanged(QString, QVariant))
			);
}

void CursorGraphic::draw()
{
	QColor color = propertyContainer_.getPropertyValue("Color").value<QColor>();

	QImage image(9,9,QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	
	p.drawLine(4,0,4,8);
	p.drawLine(0,4,8,4);

	p.end();
	image_ = image;

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

//! \brief Change the position of the cursor every frame if we have a position channel
void CursorGraphic::updateAnimation(int frame, QTime elapsedTime)
{
	Q_UNUSED(frame);
	Q_UNUSED(elapsedTime);

	if(shouldUpdateCompositingSurfaces_)
		updateCompositingSurfaces();
	
	if(positionChannel_)
	{
		int x = (int)positionChannel_->peekValue("xpos");
		int y = (int)positionChannel_->peekValue("ypos");

		setPosition(x,y);
	}
}

VisualElement* CursorGraphic::NewVisualElement()
{
	//! \TODO Figure out what to do in the NewVisualElement function
	// This function is used during deserialization, so we won't have access to a 
	// signal channel at that point.  If we wanted to use a cursor graphic
	// in a state machine, connecting it might not be possible.  However, since this
	//is not currently an issue (this is destined for use in a slave state machine)
	//I'm going to ignore this issue.
	return new CursorGraphic(QSharedPointer<SignalChannel>());
}

void CursorGraphic::slotPropertyValueChanged(QString propertyName,
											  QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}


}; //namespace Picto
