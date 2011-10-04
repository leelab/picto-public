#ifndef LEGACYSYSTEMXPANALOGSIGNALCHANNEL_H
#define LEGACYSYSTEMXPANALOGSIGNALCHANNEL_H


//#include "../common.h"
#include "../../common/engine/SignalChannel.h"

namespace Picto {

/*! \brief A SignalChannel object for collecting analog input data.  Only runs on LegacySystemXP
 *	
 *	This SignalChannel is designed to collect analog data from the inputs to the 
 *	NIDAQ board in the LegacySystem.
 *
 *	The following assumptions are made in the code:
 *	- LegacySystem is running Windows XP
 *	- LegacySystem has a PCI-MIO-16XE-50 card installed, with some number of AI channels connected
 *	- The DAQ card is "Dev2"
 *	- The development box has the NIDAQmx libraries and headers installed
 */

class LegacySystemXPAnalogInputSignalChannel : public SignalChannel
{
public:
	LegacySystemXPAnalogInputSignalChannel(int sampsPerSecond);
	~LegacySystemXPAnalogInputSignalChannel() { stop(); };

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

	double startTime_;
	int sampsCollected_;
	bool hasDevice_;

};

};

#endif