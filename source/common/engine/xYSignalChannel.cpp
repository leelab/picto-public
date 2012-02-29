#include <QPoint>
#include <QCursor>

#include <QDebug>

#include "XYSignalChannel.h"
#include "../timing/Timestamper.h"
#include "../memleakdetect.h"
#include "../memleakdetect.h"
using namespace Picto;

XYSignalChannel::XYSignalChannel(QString name,QSharedPointer<InputPort> port)
	: SignalChannel(name,4,port)
{
	//add our subchannels to the list
	addSubchannel("x",0);
	addSubchannel("y",1);
	useScaleFactors_ = false;
}

XYSignalChannel::XYSignalChannel(QString name,int xChan, int yChan, int msPerSample,QSharedPointer<InputPort> port)
	: SignalChannel(name,msPerSample,port)
{
	//add our subchannels to the list
	addSubchannel("x",xChan);
	addSubchannel("y",yChan);
}