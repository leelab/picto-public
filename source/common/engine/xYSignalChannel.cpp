#include <QPoint>
#include <QCursor>

#include <QDebug>

#include "XYSignalChannel.h"
#include "../timing/Timestamper.h"
#include "../memleakdetect.h"
#include "../memleakdetect.h"
using namespace Picto;

XYSignalChannel::XYSignalChannel(QString name)
	: SignalChannel(name)
{
	//add our subchannels to the list
	addSubchannel("x");
	addSubchannel("y");
	addSubchannel("time");
	useScaleFactors_ = false;
}

XYSignalChannel::XYSignalChannel(QString name, int sampsPerSecond)
	: SignalChannel(name,sampsPerSecond)
{
	//add our subchannels to the list
	addSubchannel("x");
	addSubchannel("y");
	addSubchannel("time");
}