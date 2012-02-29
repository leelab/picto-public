#include <QPoint>
#include <QCursor>

#include <QDebug>
#include <QCoreApplication>

#include "MouseInputPort.h"
#include "../timing/Timestamper.h"
#include "../memleakdetect.h"
#include "../memleakdetect.h"


namespace Picto {

MouseInputPort::MouseInputPort(QWidget *widget)
	: InputPort(),
	  widget_(widget)
{
	lastTime_ = -1;
}

bool MouseInputPort::startSampling()
{
	Timestamper stamper;
	lastTime_ = stamper.stampSec();
	return true;
}

void MouseInputPort::stopSampling()
{
}

//In this case, we are constantly polling the mouse position and 
//updating the data buffer, so update data buffer doesn't need to
//do anything.  However, if the timers aren't working (for example if
//we're running without an event loop), we'll call pollMouse anyway).
double MouseInputPort::updateDataBuffer()
{
	Timestamper stamper;
	double curr = stamper.stampSec();
	double samplePeriod = 1/deviceSampRate_;
	
	////This code is used to simulate application run speed and sample period errors
	////for checking InputPort synchronization code.
	//double delay = (double(rand())/RAND_MAX)*.01;	//Add random delay before sampling for test
	//while(stamper.stampSec() - curr < delay)
	//	QCoreApplication::processEvents();
	//curr = stamper.stampSec();
	//double samplePeriodError = (-1+double(rand())/(.5*RAND_MAX))*.00001;	//Add random sample rate error for test
	//samplePeriod += samplePeriodError;


	QPoint mousePos = pollMouse();
	int numSamps = (curr-lastTime_)/samplePeriod;

	//Loop through channelBuffer vectors adding latest mouse pos for all
	//new entries.
	//Actually reading mouse data at the correct interval would mean
	//multi-threading which we don't want to do for the time being
	//since it messes up the system frame rate to have multiple threads.
	//Mouse tasks never need precise timing anyway, so this shouldn't be
	//an issue.  
	QHash<int, QVector<double>>::iterator it;
	int oldSize;
	for(it = channelBuffers_.begin();it != channelBuffers_.end(); it++)
	{
		oldSize = it->size();
		it->resize(oldSize+numSamps);
		//Even channels get x data, odd channels get y data.
		if(it.key() % 2)
		{
			for(int i=oldSize;i<it->size();i++)
			{
				(*it)[i] = mousePos.y();
			}
		}
		else
		{
			for(int i=oldSize;i<it->size();i++)
			{
				(*it)[i] = mousePos.x();
			}
		}
	}
	//Update the lastTime_ value to the last sample written.
	lastTime_ = lastTime_+(numSamps * samplePeriod);
	return curr;
}

QPoint MouseInputPort::pollMouse()
{
	Timestamper stamper;
	QPoint point = QCursor::pos();
	return widget_->mapFromGlobal(point);
}

};