#ifndef PICTOBOXANALOGSIGNALCHANNEL_H
#define PICTOBOXANALOGSIGNALCHANNEL_H

#include "../common.h"
#include "../iodevices/PictoBoxdaqBoard.h"
#include "SignalChannel.h"


#include <QSharedPointer>

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API PictoBoxAnalogInputSignalChannel : public SignalChannel
#else
class PictoBoxAnalogInputSignalChannel : public SignalChannel
#endif
{
public:
	PictoBoxAnalogInputSignalChannel(QSharedPointer<PictoBoxDaqBoard> daqBoard_, int sampsPerSecond);
	~PictoBoxAnalogInputSignalChannel() {};

	bool start();
	bool stop();
	void updateDataBuffer();

	//adds a subcahnnel to the signal channel.  The subchannel will contain data
	//from the DAQ board's AI channel with the number aiChannelNum
	void addAiChannel(QString subchannelName, int aiChannelNum);

private:

	QSharedPointer<PictoBoxDaqBoard> daqBoard_;

	//maps channel name to daq board ai channel number
	QMap<QString, int> aiChannelNums_;  

};

};

#endif