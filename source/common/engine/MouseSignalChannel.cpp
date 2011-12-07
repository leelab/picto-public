#include <QPoint>
#include <QCursor>

#include <QDebug>

#include "MouseSignalChannel.h"
#include "../timing/Timestamper.h"
#include "../memleakdetect.h"
#include "../memleakdetect.h"


namespace Picto {

MouseSignalChannel::MouseSignalChannel(int sampsPerSecond, QWidget *widget)
	: SignalChannel(sampsPerSecond),
	  widget_(widget)
{
	//add our subchannels to the list
	addSubchannel("xpos");
	addSubchannel("ypos");
	addSubchannel("time");

	//pollingTimer_.setInterval(1000/sampsPerSecond);
	//connect(&pollingTimer_, SIGNAL(timeout()), this, SLOT(pollMouse()));
}

bool MouseSignalChannel::start()
{
	//pollingTimer_.start();
	return true;
}

bool MouseSignalChannel::stop()
{
	//pollingTimer_.stop();
	return true;
}

//In this case, we are constantly polling the mouse position and 
//updating the data buffer, so update data buffer doesn't need to
//do anything.  However, if the timers aren't working (for example if
//we're running without an event loop), we'll call pollMouse anyway).
void MouseSignalChannel::updateDataBuffer()
{
	pollMouse();
}
void MouseSignalChannel::pollMouse()
{
	Timestamper stamper;
	QPoint point = QCursor::pos();
	point = widget_->mapFromGlobal(point);

	rawDataBuffer_["xpos"].append((double)point.x());
	rawDataBuffer_["ypos"].append((double)point.y());
	rawDataBuffer_["time"].append(stamper.stampSec());
}

};