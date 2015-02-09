#include <QPoint>
#include <QCursor>

#include <QDebug>

#include "XYSignalChannel.h"
#include "../timing/Timestamper.h"
#include "../memleakdetect.h"

using namespace Picto;

/*! \brief COnstructs an XYSignalChannel with the input name and InputPort.  Sample period is set to 4ms.
*/
XYSignalChannel::XYSignalChannel(QString name,QSharedPointer<InputPort> port)
	: SignalChannel(name,4,port)
{
	//add our subchannels to the list
	addSubchannel("x",0);
	addSubchannel("y",1);
	useScaleFactors_ = false;
}

/*! \brief COnstructs an XYSignalChannel
 *	\details name is the SignalChannel name. xChan and yChan are channel numbers of the x and y channels in the input port respectively and they
 *	will be added in this constructor using addSubchannel().  msPerSample is the sample period.  port in the InputPort from which raw data will
 *	be gathered.  In the case of the TestViewer, port would be a MouseInputPort.  In a live experiment on a Pictobox, port would be a 
 *	PictoBoxXPAnalogInputPort.
*/
XYSignalChannel::XYSignalChannel(QString name,int xChan, int yChan, int msPerSample,QSharedPointer<InputPort> port)
	: SignalChannel(name,msPerSample,port)
{
	//add our subchannels to the list
	addSubchannel("x",xChan);
	addSubchannel("y",yChan);
}