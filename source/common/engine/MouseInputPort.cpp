#include <QPoint>
#include <QCursor>

#include <QDebug>
#include <QCoreApplication>

#include "MouseInputPort.h"
#include "../timing/Timestamper.h"
#include "../compositor/VisualTarget.h"
#include "../compositor/VisualTargetHost.h"
#include "../memleakdetect.h"


namespace Picto {

/*! \brief Constructs a new MouseInputPort.
 *	\details The input widget is the widget within which the mouse coordinates returned by this function should be defined.
 *	Typically, this would be the VisualTarget.
 */
MouseInputPort::MouseInputPort(QWidget *widget)
	: InputPort(), widget_(widget), visualTarget(nullptr), visualTargetHost(nullptr)
{
	lastTime_ = -1;


	visualTarget = qobject_cast<VisualTarget *>(widget);
	visualTargetHost = qobject_cast<VisualTargetHost *>(widget);
}

/*! \brief Initializes the mouse port for sampling mouse data.
 *	\details There is no initialization necessary to start reading mouse data from the OS.  However, since in the mouse
 *	port we simulate sampling at the sample rate configured in InputPort, we need to mark the startSampling time in order
 *	to calculate the number of simulated samples that "have occured" when updateDataBuffer() is called.
 */
bool MouseInputPort::startSampling()
{
	Timestamper stamper;
	lastTime_ = stamper.stampSec();
	return true;
}

/*! \brief Deinitializes mouse sampling.
 *	\details In practice, there is no deinitialization necessary, so we leave this blank.
 */
void MouseInputPort::stopSampling()
{
}

/*! \copydoc InputPort::updateDataBuffer()
 *	\details In the case of the MouseInputPort, there is no external hardware system or any external data buffer that is
 *	filled and needs to be copied in.  Also, whenever the mouse is used as the input, precise timing is unnecessary.
 *	For this reason, we "simulate" reading in mouse data at the correct sample rate.  We actually just check the mouse
 *	position once for every time that this function is called, but fake additional samples are produced to fill out the
 *	correct number of samples based on the configured sample rate and elapsed time since this function was last called.
 */
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

	//Loop through channelBuffer vectors adding latest mouse pos for all new entries.
	//	Actually reading mouse data at the correct interval would mean multi-threading which we don't want to do for the
	//	time being since it messes up the system frame rate to have multiple threads.  Mouse tasks never need precise
	//	timing anyway, so this shouldn't be an issue.  
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

/*! \brief Returns a QPoint with the current mouse position in the coordinate frame of the widget passed into the
 *	constructor.
 */
QPoint MouseInputPort::pollMouse()
{
	Timestamper stamper;
	QPoint point = QCursor::pos();
	QPoint translated = widget_->mapFromGlobal(point);

	if (visualTarget)
	{
		return visualTarget->viewportPointToTargetPoint(translated);
	}
	else if (visualTargetHost)
	{
		return visualTargetHost->viewportPointToTargetPoint(translated);
	}
	
	return translated;
}

};