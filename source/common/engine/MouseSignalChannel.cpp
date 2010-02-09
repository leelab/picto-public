#include <QPoint>
#include <QCursor>

#include "MouseSignalChannel.h"

namespace Picto {

MouseSignalChannel::MouseSignalChannel(int sampsPerSecond, QSharedPointer<QWidget> widget_)
	: SignalChannel(sampsPerSecond),
	  widget_(widget_)
{
	//add our subchannels to the list
	addSubchannel("xpos");
	addSubchannel("ypos");

	//Since this is coordinate data, there doesn't need to be any calibration

	pollingTimer_ = new QTimer(this);
	pollingTimer_->setInterval(1000/sampsPerSecond);
	connect(pollingTimer_, SIGNAL(timeout()), this, SLOT(pollMouse()));
}

bool MouseSignalChannel::start()
{
	pollingTimer_->start();
	return true;
}

bool MouseSignalChannel::stop()
{
	pollingTimer_->stop();
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
	point = widget_->mapFromGlobal(point);
	rawDataBuffer_["xpos"].append((double)point.x());
	rawDataBuffer_["ypos"].append((double)point.y());
	
}

};