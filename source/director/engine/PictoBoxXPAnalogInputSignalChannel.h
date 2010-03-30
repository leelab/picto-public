#ifndef PICTOBOXXPANALOGSIGNALCHANNEL_H
#define PICTOBOXXPANALOGSIGNALCHANNEL_H


//#include "../common.h"
#include "../../common/engine/SignalChannel.h"

/*! \brief A SignalChannel object for collecting analog input data.  Only runs on PictoBoxXP
 *	
 *	This SignalChannel is designed to collect analog data from the inputs to the 
 *	NIDAQ board in the PictoBox.
 *
 *	The following assumptions are made in the code:
 *	- PictoBox is running Windows XP
 *	- PictoBox has a PCI-6221 card installed, with some number of AI channels connected
 *	- The DAQ card is "Dev1"
 *	- The development box has the NIDAQmx libraries and headers installed
 */

namespace Picto {

//#if defined WIN32 || defined WINCE
//class PICTOLIB_API PictoBoxXPAnalogInputSignalChannel : public SignalChannel
//#else
class PictoBoxXPAnalogInputSignalChannel : public SignalChannel
//#endif
{
public:
	PictoBoxXPAnalogInputSignalChannel(int sampsPerSecond);
	~PictoBoxXPAnalogInputSignalChannel() { stop(); };

	bool start();
	bool stop();
	void updateDataBuffer();

	//adds a subcahnnel to the signal channel.  The subchannel will contain data
	//from the DAQ board's AI channel with the number aiChannelNum
	void addAiChannel(QString subchannelName, int aiChannelNum);

private:

	unsigned long  daqTaskHandle_;

	//maps channel name to daq board ai channel number
	QMap<QString, int> aiChannelNums_;  

};

};

#endif