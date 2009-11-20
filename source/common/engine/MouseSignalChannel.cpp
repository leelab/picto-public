#include <QPoint>
#include <QCursor>

#include "MouseSignalChannel.h"

namespace Picto {

MouseSignalChannel::MouseSignalChannel(int sampsPerSecond, QWidget *widget)
	: SignalChannel(sampsPerSecond),
	  widget(widget)
{
	//add our subchannels to the list
	QList<double> list;
	rawDataBuffer["xpos"]= list;
	rawDataBuffer["ypos"]= list;

	pollingTimer = new QTimer(this);
	pollingTimer->setInterval(1000/sampsPerSecond);
	connect(pollingTimer, SIGNAL(timeout()), this, SIGNAL(pollMouse()));
}

bool MouseSignalChannel::start()
{
	pollingTimer->start();
	return true;
}

bool MouseSignalChannel::stop()
{
	pollingTimer->stop();
	return true;
}

//In this case, we are constantly polling the mouse position and 
//updating the data buffer, so update data buffer doesn't need to
//do anything.
void MouseSignalChannel::updateDataBuffer()
{
}

void MouseSignalChannel::pollMouse()
{
	QPoint point = QCursor::pos();
	point = widget->mapFromGlobal(point);
	rawDataBuffer["xpos"].append((double)point.x());
	rawDataBuffer["ypos"].append((double)point.y());
	
}

};