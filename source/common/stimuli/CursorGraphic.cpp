#include <QPainter>
#include "CursorGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

const QString CursorGraphic::type = "Cursor Graphic";

CursorGraphic::CursorGraphic(QSharedPointer<SignalChannel> channel, QColor color)
: VisualElement(QPoint(0,0),color)
{
	// This is never serialized, so I didn't update it to use the new serialization routine.
	// It does bring to light the fact that most of our Asset objects must be deserialized
	// to have valid contents, which might be something worth fixing.
	positionChannel_ = channel;
	if(positionChannel_)
	{
		Q_ASSERT(positionChannel_->getSubchannels().contains("xpos"));
		Q_ASSERT(positionChannel_->getSubchannels().contains("ypos"));
	}
	propertyContainer_->addProperty(QVariant::String,"Name","");
	propertyContainer_->setContainerName(type);
	initializePropertiesToDefaults();
	propertyContainer_->setPropertyValue("Position",QPoint(0,0));
	propertyContainer_->setPropertyValue("Color",color);

	draw();

	//connect(propertyContainer_.data(),
	//	    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
	//	    this,
	//		SLOT(slotPropertyValueChanged(QString, int, QVariant))
	//		);
}

void CursorGraphic::draw()
{
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(14,14,QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	QPen pen(color);
	pen.setWidth(2);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setPen(pen);
	p.setBrush(color);
	
	p.drawLine(7,0,7,13);
	p.drawLine(0,7,13,7);

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
		//In the director, every time the positionChannel values are sent to the server they are cleared.
		//We need something in the remoteViewer to clear the positionChannel values as well.
		//For this purpose, we call clear here since the Cursor Graphic is the only thing in the
		//Remote viewer to use the positionChannel anyway.  We check if we're running on the remote
		//viewer by seeing if this positionChannel is a network signal channel.
		if(positionChannel_->inherits("Picto::NetworkSignalChannel"))
			positionChannel_->clearValues();	

		setPosition(QPoint(x,y));
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

//void CursorGraphic::slotPropertyValueChanged(QString propertyName, int,
//											  QVariant) //propertyValue
//{
//	if(propertyName != "Position" && propertyName != "Name")
//	{
//		draw();
//	}
//}

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
