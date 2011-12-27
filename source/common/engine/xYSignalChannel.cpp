#include <QPoint>
#include <QCursor>

#include <QDebug>

#include "XYSignalChannel.h"
#include "../timing/Timestamper.h"
#include "../memleakdetect.h"
#include "../memleakdetect.h"
using namespace Picto;

XYSignalChannel::XYSignalChannel()
	: SignalChannel()
{
	//add our subchannels to the list
	addSubchannel("xpos");
	addSubchannel("ypos");
	addSubchannel("time");
}

XYSignalChannel::XYSignalChannel(int sampsPerSecond)
	: SignalChannel(sampsPerSecond)
{
	//add our subchannels to the list
	addSubchannel("xpos");
	addSubchannel("ypos");
	addSubchannel("time");
}